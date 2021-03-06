<?php

/**
 * @file
 * preprocess.inc
 */

/**
 * Implements hook_preprocess_html().
 * Override or insert variables into the html template.
 *
 * @param $variables
 * @param $hook
 */
function govstrap_preprocess_html(&$variables, $hook) {
  // Set common variables.
  $variables['base_path'] = base_path();
  $variables['path_to_govstrap'] = drupal_get_path('theme', 'govstrap');

  // Send X-UA-Compatible HTTP header to force IE to use the most recent
  // rendering engine or use Chrome's frame rendering engine if available.
  if (is_null(drupal_get_http_header('X-UA-Compatible'))) {
    drupal_add_http_header('X-UA-Compatible', 'IE=edge');
  }

  // IE8 support of HTML5 elements and media queries .
  $ie_script_s = array(
    '#browsers' => array('IE' => 'lt IE 9', '!IE' => FALSE),
    '#tag' => 'script',
    '#attributes' => array(
      'type' => "text/javascript",
      'src' => './' . $variables['path_to_govstrap'] . '/js/modernizr.js',

    ),
  );
  drupal_add_html_head($ie_script_s, "govstrapmodernizr");

  // IE8 support of HTML5 elements and media queries.
  $ie_script_r = array(
    '#browsers' => array('IE' => 'lt IE 9', '!IE' => FALSE),
    '#tag' => 'script',
    '#attributes' => array(
      'type' => "text/javascript",
      'src' => './' . $variables['path_to_govstrap'] . '/js/respond.js',
    ),
  );
  drupal_add_html_head($ie_script_r, "govstraprespond");

  if (theme_get_setting('fontawesome_enabled') && theme_get_setting('fontawesome_css_cdn')) {
    // Add fontawesome CDN file.
    $fontawesome = 'https://use.fontawesome.com/releases/' . theme_get_setting('fontawesome_css_cdn') . '/js/all.js';
    drupal_add_js($fontawesome, array('type' => 'external', 'defer' => TRUE));
  }

  // Attributes for html element.
  $variables['html_attributes_array'] = array(
    'lang' => $variables['language']->language,
    'dir' => $variables['language']->dir,
  );

  // Return early, so the maintenance page does not call any of the code below.
  if ($hook != 'html') {
    return;
  }

  // Serialize RDF Namespaces into an RDFa 1.1 prefix attribute.
  if ($variables['rdf_namespaces']) {
    $prefixes = array();
    foreach (explode("\n  ", ltrim($variables['rdf_namespaces'])) as $namespace) {
      // Remove xlmns: and ending quote and fix prefix formatting.
      $prefixes[] = str_replace('="', ': ', substr($namespace, 6, -1));
    }
    $variables['rdf_namespaces'] = ' prefix="' . implode(' ', $prefixes) . '"';
  }

  if (drupal_is_front_page()) {
    $variables['head_title'] = variable_get('site_name', 'National Capital Authority');
  }

}

/**
 * Implements hook_preprocess_page().
 *
 * @param $variables
 *
 * @see page.tpl.php
 */
function govstrap_preprocess_page(&$variables) {
  // Add pathToTheme to Drupal.settings in JS.
  drupal_add_js('jQuery.extend(Drupal.settings, { "pathToTheme": "' . path_to_theme() . '" });', 'inline');

  // Theme suggestions.
  $header = drupal_get_http_header("status");
  if ($header === "404 Not Found") {
    $variables['theme_hook_suggestions'][] = 'page__404';
  }
  if ($header === "403 Forbidden") {
    $variables['theme_hook_suggestions'][] = 'page__403';
  }

  // Set page container columns.
  $variables['container_class'] = 'container';

  // Calculate container columns with sidebar.
  if (!empty($variables['page']['sidebar_first']) && !empty($variables['page']['sidebar_second'])) {
    $variables['content_column_class'] = 'col-sm-4';
  }
  elseif (!empty($variables['page']['sidebar_first']) || !empty($variables['page']['sidebar_second'])) {
    $variables['content_column_class'] = 'col-sm-8';
  }
  else {
    $variables['content_column_class'] = 'col-sm-12';
  }
  // Decide if we need negative bottom margin for the highlight region
  if (!empty($variables['breadcrumb'])) {
    $variables['highlighted_class'] = 'highlighted hl-negative-bottom-margin';
  } else {
    $variables['highlighted_class'] = 'highlighted';
  }
}

/**
 * Implements hook_preprocess_node().
 */
function govstrap_preprocess_node(&$variables, $hook) {
  // Add a theme hook suggestion for type and view mode.
  $variables['theme_hook_suggestions'][] = 'node__' . $variables['type'] . '__' . $variables['view_mode'];
}

/**
 * Implements hook_preprocess_block().
 */
function govstrap_preprocess_block(&$variables, $hook) {
  // Use a template with no wrapper for the page's main content.
  if ($variables['block_html_id'] == 'block-system-main') {
    $variables['theme_hook_suggestions'][] = 'block__no_wrapper';
  }

  $variables['title_attributes_array']['class'][] = 'block-title';

  // Block template per bean type.
  if ($variables['block']->module === 'bean') {
    $beans = $variables['elements']['bean'];
    $bean_keys = element_children($beans);
    $bean = $beans[reset($bean_keys)];
    // Add template suggestions for bean types.
    $variables['theme_hook_suggestions'][] = 'block__bean__' . $bean['#bundle'];
  }
}

/**
 * Implements hook_preprocess_field().
 */
function govstrap_preprocess_field(&$vars, $hook) {

  $function = __FUNCTION__ . '__' . $vars['element']['#field_name'];
  if (function_exists($function)) {
    $function($vars, $hook);
  }

  // Override output of closing date field.
  if ($vars['element']['#field_name'] === 'field_closing_date') {
    $close_date = strtotime($vars['element']['#items'][0]['value']);
    $date = format_date($close_date, 'custom', 'j F');
    $now = time();
    $remaining_time = _time_diff($close_date, $now);

    if ($close_date < $now) {
      $text = t('Closed');
      $class = 'nca-consultation-closed';
    }
    else {
      switch ($vars['element']['#view_mode']) {

        case 'full':
          if ($remaining_time['days'] > 0) {
            $text = t('This consultation closes on @date, @days remaining.', array('@date' => $date, '@days' => format_plural($remaining_time['days'], '1 day', '@count days')));
          }
          else {
            $text = t('This consultation closes on closes @date, less than 1 day remaining.', array('@date' => $date));
          }
          break;

        case 'compact':
          if ($remaining_time['days'] > 0) {
            $text = t('@days', array('@date' => $date, '@days' => format_plural($remaining_time['days'], '1 Day', '@count Days')));
          }
          else {
            $text = t('Last Day');
          }
          break;
      }
      $class = ($remaining_time['days'] <= 3) ? 'nca-consultation-days-remaining nca-consultation-closing-soon' : 'nca-consultation-days-remaining nca-consultation-open';

    }
    $vars['items'][0]['#markup'] = '<div class="' . $class . '" title="Time remaining">' . $text . '</div>';
  }
}

function _time_diff($t1, $t2)
{
  if($t1 > $t2) {
    $time1 = $t2;
    $time2 = $t1;
  }
  else {
    $time1 = $t1;
    $time2 = $t2;
  }
  $diff = array(
    'days' => 0,
    'hours' => 0,
    'minutes' => 0,
    'seconds' =>0
  );
  foreach (array('days','hours','minutes','seconds') as $unit) {
    while (TRUE)       {
      $next = strtotime("+1 $unit", $time1);
      if($next < $time2) {
        $time1 = $next;
        $diff[$unit]++;
      }
      else {
        break;
      }
    }
  }
  return($diff);
}