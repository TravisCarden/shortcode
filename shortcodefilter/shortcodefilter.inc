<?php

/**
 * @file
 * Internal functions for the Shortcode Filter module.
 */

/**
 * Implements callback_filter_process().
 */
function _shortcodefilter_filter_process_callback($text) {
  return shortcode_process_shortcodes($text);
}

/**
 * Implements callback_filter_tips().
 */
function _shortcode_filter_tips_callback($filter, $format, $long) {
  $shortcodes = shortcode_get_shortcodes();

  if (empty($shortcodes)) {
    return t('No available shortcodes.');
  }

  $tag_list = array_keys($shortcodes);
  $output = t('Available shortcodes: [@tags]', array(
    '@tags' => implode('] [', $tag_list),
  ));

  // Return a more elaborate filter tip for theme_filter_tips().
  if ($long) {
    $output = '<p>' . $output . '</p>';
    $output .= '<p>' . t('Shortcodes provide a way to safely embed code within content without having to know HTML. This table provides examples for each shortcode that is enabled on this site:') . '</p>';

    $header = array(t('Shortcode description'), t('Syntax'));
    $rows = array();
    foreach ($shortcodes as $tag => $definition) {
      $rows[] = array($definition['title'], "<code>{$definition['syntax']}</code>");
    }
    $output .= theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ));

    return $output;
  }
  // Return a short tip to display in a form.
  else {
    return $output;
  }

}
