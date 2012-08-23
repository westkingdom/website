<?php 
/**
 * @file
 * Alpha's theme implementation to display a single Drupal page.
 */
?>
<div<?php print $attributes; ?>>
  <div id="outer-wrapper" class="container-12">
    <div id="supporter-wrapper">
      <div id="supporter_l">
        <div id="supporter_r">
          <div id="edge_l">
            <div id="edge_r">
              <div id="inner-wrapper">
                <?php if (isset($page['header'])) : ?>
                  <?php print render($page['header']); ?>
                <?php endif; ?>

                <?php if (isset($page['content'])) : ?>
                  <?php print render($page['content']); ?>
                <?php endif; ?>  

                <?php if (isset($page['footer'])) : ?>
                  <?php print render($page['footer']); ?>
                <?php endif; ?>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
