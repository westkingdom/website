
jQuery(document).ready(function($) {

  // ga: If we hide our pane here, then the om-maximenu contents are not initialized,
  // and our pane is empty when it opens.  :(  n.b. this is only true if we use
  // one of the 'slide' effects.  If we use 'no effect', everything is okay.
  $("#om-maximenu-om-user-region .om-maximenu-tabbed-content-outer").hide();
  $('#om-maximenu-om-user-region #om-close').hide();

  $("#om-close").click(function(){
    $('#om-maximenu-om-user-region #om-close').fadeOut('slow');
    $("#om-maximenu-om-user-region .om-maximenu-tabbed-content-outer").slideUp("slow");
  });

  $('#om-maximenu-om-user-region .om-leaf .om-link').click(function () {
    $('#om-maximenu-om-user-region .om-maximenu-tabbed-content-inner').hide();
    $('#om-maximenu-om-user-region .om-maximenu-tabbed-content-inner').fadeIn('slow');
    $('#om-maximenu-om-user-region #om-close').fadeIn('slow');
    $('#om-maximenu-om-user-region .om-maximenu-tabbed-content-outer').slideDown('slow');
  });

});
