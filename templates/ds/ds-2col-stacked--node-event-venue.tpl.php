<?php

/**
 * @file
 * Display Suite 2 column stacked template.
 */
?>
<<?php print $layout_wrapper;
print $layout_attributes; ?> class="ds-event-venue ds-2col-stacked <?php print $classes; ?> clearfix">

<?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
<?php endif; ?>

<<?php print $header_wrapper ?> class="group-header<?php print $header_classes; ?>">
<?php print $header; ?>
</<?php print $header_wrapper ?>>
<div class="ds-2-col">
    <<?php print $left_wrapper ?> class="col-left col-md-6<?php print $left_classes; ?>">
  <?php print $left; ?>
</<?php print $left_wrapper ?>>

<<?php print $right_wrapper ?> class="col-right col-md-6<?php print $right_classes; ?>">
<?php print $right; ?>
</<?php print $right_wrapper ?>>
</div>

<<?php print $footer_wrapper ?> class="group-footer<?php print $footer_classes; ?>">
<?php print $footer; ?>
</<?php print $footer_wrapper ?>>

</<?php print $layout_wrapper ?>>

<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
