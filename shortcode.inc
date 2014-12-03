<?php

/**
 * @file
 * Internal functions for the Shortcode module.
 */

/**
 * Determines whether a given shortcode definition is valid or not.
 *
 * @param array $definition
 *   A shortcode definition as registered with hook_shortcode_info().
 *
 * @return bool
 *   Returns TRUE if the given shortcode definition is valid, or FALSE if not.
 */
function _shortcode_is_valid_shortcode(array $definition) {
  // A shortcode must contain a title.
  if (empty($definition['title'])) {
    return FALSE;
  }

  // A shortcode must contain a syntax example.
  if (empty($definition['syntax'])) {
    return FALSE;
  }

  // A shortcode must contain a valid process callback.
  if (empty($definition['process callback']) || !is_callable($definition['process callback'])) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Converts a string of shortcode attributes into an array.
 *
 * @param string $text
 *   A string of XML/HTML attributes. "Bare" or value-less attributes are not
 *   supported.
 *
 * @return array
 *   An associative array of key/value pairs corresponding to the attributes
 *   given.
 */
function _shortcode_parse_attributes($text) {
  $attributes = array();
  $pattern = '/(\w+)\s*=\s*"([^"]*)"(?:\s|$)|(\w+)\s*=\s*\'([^\']*)\'(?:\s|$)|(\w+)\s*=\s*([^\s\'"]+)(?:\s|$)|"([^"]*)"(?:\s|$)|(\S+)(?:\s|$)/';
  $text = preg_replace("/[\x{00a0}\x{200b}]+/u", " ", $text);
  if (preg_match_all($pattern, $text, $matches, PREG_SET_ORDER)) {
    foreach ($matches as $match) {
      if (!empty($match[1])) {
        $attributes[strtolower($match[1])] = stripcslashes($match[2]);
      }
      elseif (!empty($match[3])) {
        $attributes[strtolower($match[3])] = stripcslashes($match[4]);
      }
      elseif (!empty($match[5])) {
        $attributes[strtolower($match[5])] = stripcslashes($match[6]);
      }
      elseif (isset($match[7]) and strlen($match[7])) {
        $attributes[] = stripcslashes($match[7]);
      }
      elseif (isset($match[8])) {
        $attributes[] = stripcslashes($match[8]);
      }
    }
  }
  return $attributes;
}

/**
 * Regex replacement callback for shortcode_process_shortcodes().
 *
 * @param array $matches
 *   An array of regular expression matches as passed by
 *   preg_replace_callback().
 *
 * @return string
 *   Returns a processed shortcode.
 *
 * @see _shortcode_shortcode_regex()
 */
function _shortcode_process_tag(array $matches) {
  // If the tag is escaped using double bracket syntax (e.g., [[tag]]), strip
  // the outermost brackets ([]) and return the inner tag un-processed.
  if ($matches[1] == '[' && $matches[6] == ']') {
    return substr($matches[0], 1, -1);
  }

  $tag = $matches[2];
  $attributes = _shortcode_parse_attributes($matches[3]);
  $content = $matches[5];

  // Invoke callback_shortcode_process() and return.
  $all_tags = shortcode_get_shortcodes();
  return $matches[1] . call_user_func($all_tags[$tag]['process callback'], $tag, $attributes, $content) . $matches[6];
}

/**
 * Returns the regular expression for matching a given list of shortcodes.
 *
 * This regex supports six features of a shortcode:
 *
 * -# An optional extra opening bracket ([) to allow for escaping shortcodes
 *    with double brackets ([[]]).
 * -# The shortcode tag name.
 * -# Optional shortcode attributes.
 * -# An optional forward slash (/) for a self-closing tag.
 * -# Optional shortcode tag content, i.e., the text between opening and closing
 *    tags, if any.
 * -# An optional extra closing bracket (]) to allow for escaping shortcodes
 *    with double brackets ([[]]).
 *
 * _shortcode_process_tag() is tightly coupled to this regular expression, so
 * if the one is ever changed, the other must be also.
 *
 * @param array $tags
 *   An indexed array of tag names.
 *
 * @return string
 *   The regular expression for matching shortcodes.
 */
function _shortcode_shortcode_regex(array $tags) {
  $tag_subpattern = implode('|', array_map('preg_quote', $tags));

  // A complicated regular expression is one of the few places that we can
  // actually improve readability by violating coding standards.
  // @codingStandardsIgnoreStart
  return ''
  . '\\['                      // An opening bracket ([).
  . '(\\[?)'                   // 1. Optional second opening bracket for escaping shortcodes, e.g., [[tag]].
  . "({$tag_subpattern})"      // 2. Shortcode name.
  . '(?![\\w-])'               // Not followed by a word character or hyphen.
  . '('                        // 3. Unroll the loop: inside the opening shortcode tag.
  .   '[^\\]\\/]*'             // Not a closing bracket (]) or forward slash (/).
  .   '(?:'
  .     '\\/(?!\\])'           // A forward slash (/) not followed by a closing bracket (]).
  .     '[^\\]\\/]*'           // Not a closing bracket (]) or forward slash (/).
  .   ')*?'
  . ')'
  . '(?:'
  .   '(\\/)'                  // 4. Self closing tag...
  .   '\\]'                    // ...and closing bracket (]).
  . '|'
  .   '\\]'                    // Closing bracket (]).
  .   '(?:'
  .     '('                    // 5. Unroll the loop: optionally, anything between the opening and closing shortcode tags.
  .       '[^\\[]*+'           // Not an opening bracket ([).
  .       '(?:'
  .         '\\[(?!\\/\\2\\])' // An opening bracket ([) not followed by the closing shortcode tag.
  .         '[^\\[]*+'         // Not an opening bracket ([).
  .       ')*+'
  .     ')'
  .     '\\[\\/\\2\\]'         // Closing shortcode tag.
  .   ')?'
  . ')'
  . '(\\]?)';                  // 6. Optional second closing bracket for escaping shortcodes: [[tag]].
  // @codingStandardsIgnoreEnd
}
