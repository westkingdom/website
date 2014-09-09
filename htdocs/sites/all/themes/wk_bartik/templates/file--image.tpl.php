<?php

/**
 * @file
 * Default theme implementation to display a file.
 *
 * Available variables:
 * - $label: the (sanitized) file name of the file.
 * - $content: An array of file items. Use render($content) to print them all,
 *   or print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $user_picture: The file owner's picture from user-picture.tpl.php.
 * - $date: Formatted added date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $timestamp
 *   variable.
 * - $name: Themed username of file owner output from theme_username().
 * - $file_url: Direct URL of the current file.
 * - $display_submitted: Whether submission information should be displayed.
 * - $submitted: Submission information created from $name and $date during
 *   template_preprocess_file().
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - file-entity: The current template type, i.e., "theming hook".
 *   - file-[type]: The current file type. For example, if the file is a
 *     "Image" file it would result in "file-image". Note that the machine
 *     name will often be in a short form of the human readable label.
 *   - file-[mimetype]: The current file's MIME type. For exampe, if the file
 *     is a PNG image, it would result in "file-image-png"
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Other variables:
 * - $file: Full file object. Contains data that may not be safe.
 * - $type: File type, i.e. image, audio, video, etc.
 * - $uid: User ID of the file owner.
 * - $timestamp: Time the file was added formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   listings.
 * - $id: Position of the file. Increments each time it's output.
 *
 * File status variables:
 * - $view_mode: View mode, e.g. 'default', 'full', etc.
 * - $page: Flag for the full page state.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * Field variables: for each field instance attached to the file a corresponding
 * variable is defined, e.g. $file->caption becomes $caption. When needing to
 * access a field's raw values, developers/themers are strongly encouraged to
 * use these variables. Otherwise they will have to explicitly specify the
 * desired field language, e.g. $file->caption['en'], thus overriding any
 * language negotiation rule that was previously applied.
 *
 * @see template_preprocess()
 * @see template_preprocess_file_entity()
 * @see template_process()
 *
 * Things I did:
 *
 *  - create a new file field "Page picture"
 *  - make it a "rendered file" and make its view mode "full width"
 *  - make the view mode for the direction diagram "full width" too
 *  - create an "image style" field of type list (text) attached to image files
 *  - give it allowed values of full width, half left, half right, etc.
 *  - make sure image style field is not shown in any view mode
 *
 */

$image_style_css = 'image-style-full';
// n.b. the comment above implies that $field_image_style[LANGUAGE_NONE][0]['value']
// should be available as simply '$field_image_style', but this does not appear
// to be the case.
if (!$is_front && isset($field_image_style[LANGUAGE_NONE][0]['value'])) {
  if ($field_image_style[LANGUAGE_NONE][0]['value'] == 'default') {
    if (isset($file->referencing_entity) && ($file->referencing_entity->type == 'location')) {
      $image_style_css = 'media-right';
    }
    else {
      $image_style_css = 'media-left';
    }
  }
  else {
    $image_style_css = 'media-' . $field_image_style[LANGUAGE_NONE][0]['value'];
  }
}
if (!$is_front && isset($field_image_width[LANGUAGE_NONE][0]['value'])) {
  $image_style_css .= ' media-width-' . $field_image_width[LANGUAGE_NONE][0]['value'];
}

$link_prefix = '';
$link_suffix = '';
if(isset($field_link[LANGUAGE_NONE][0]['url'])) {
  $link_suffix = '</a>';
  $link_prefix = '<a href="' . $field_link[LANGUAGE_NONE][0]['url'] . '">';
}
?>
<div id="file-<?php print $file->fid ?>" class="<?php print $classes ?>"<?php print $attributes; ?>>

  <?php if (!$page): ?>
    <?php print render($title_prefix); ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $file_url; ?>"><?php print $label; ?></a></h2>
    <?php print render($title_suffix); ?>
  <?php endif; ?>

  <?php if ($display_submitted): ?>
    <div class="submitted">
      <?php print $submitted; ?>
    </div>
  <?php endif; ?>

  <div class="image-frame <?php print $image_style_css; ?>">
  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the links now so that we can render them later.
      hide($content['links']);
      $content['file']['#prefix'] = $link_prefix;
      $content['file']['#suffix'] = $link_suffix;
      $content['field_file_image_title_text'][0]['#markup'] = $link_prefix . $content['field_file_image_title_text'][0]['#markup'] . $link_suffix;
      print render($content);
    ?>
  </div>
  </div>

  <?php print render($content['links']); ?>

</div>
