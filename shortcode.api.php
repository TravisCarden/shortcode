<?php

/**
 * @file
 * Hooks provided by the Shortcode module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define shortcodes and their process callbacks.
 *
 * This hook is invoked by shortcode_get_shortcodes() and allows modules to
 * register shortcodes they provide.
 *
 * @return array
 *   An associative array of shortcode definitions, where each key is a tag name
 *   and its corresponding value is a multidimensional array describing the
 *   shortcode with the following key-value pairs:
 *   - title: (required) A translated name for the shortcode.
 *   - syntax: (required) An example use of the shortcode, showing basic syntax
 *     and supported attributes. This is displayed by Shortcode Filter in filter
 *     tips.
 *   - process callback: (required) The name of the function that performs the
 *     shortcode expansion. See callback_shortcode_process() for details.
 */
function hook_shortcode_info() {
  $shortcodes['example'] = array(
    'title' => t('Example'),
    'syntax' => '[example foo="foo val" bar="bar val"]content[/example]',
    'process callback' => 'example_process_callback',
  );
  return $shortcodes;
}

/**
 * Perform alterations on shortcode definitions.
 *
 * @param array $shortcodes
 *   An array of shortcode definitions exposed by hook_shortcode_info()
 *   implementations.
 */
function hook_shortcode_info_alter(array &$shortcodes) {
  // Replace the "example" shortcode process callback.
  $shortcodes['example']['process callback'] = 'some_other_callback';

  // Remove the "example" shortcode definition altogether.
  unset($shortcodes['example']);
}

/**
 * @} End of "addtogroup hooks".
 */

/**
 * Process callback for hook_shortcode_info().
 *
 * See hook_shortcode_info() for a description of the process. This step is
 * where the shortcode is actually expanded.
 *
 * @param string $tag
 *   The name of the shortcode tag. This is useful if you use the same process
 *   callback for multiple shortcodes.
 * @param array $attributes
 *   An associative array of XML/HTML tag attributes suitable for use with
 *   drupal_attributes().
 * @param string $content
 *   The content of the shortcode tag, i.e., the text between the opening and
 *   closing tags. This will be an empty string in the case of a self-closing
 *   tag.
 *
 * @return string
 *   The processed shortcode text.
 *
 * @ingroup callbacks
 */
function callback_shortcode_process($tag, array $attributes, $content) {
  // Reduce the given attributes to an allowed subset. Note: This only removes
  // disallowed attributes. It does not sanitize remaining attribute values. For
  // security, the result must still be output with drupal_attributes() or
  // individual values run through check_plain() before used.
  $allowed_list = array('foo', 'bar', 'baz');
  $allowed_attributes = shortcode_filter_attributes($attributes, $allowed_list);

  // Nested shortcodes are not handled automatically. If you want to support
  // them, you must manually process the $content variable.
  $processed_content = shortcode_process_shortcodes($content);

  return '<' . $tag . drupal_attributes($allowed_attributes) . '>' . $processed_content . '</' . $tag . '>';
}
