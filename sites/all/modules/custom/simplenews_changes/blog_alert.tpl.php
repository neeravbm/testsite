<?php

global $base_url;
$now = time();

$trimmed = node_view($blog_entry, 'teaser');

?>
<div>The following article has been added to the Simplicity 2.0 blog (from Laserfiche):</div>
<h3 class="blog-title"><a href="<?php print $base_url; ?>/node/<?php print $blog_entry->nid; ?>"><?php print check_plain($blog_entry->title); ?></a></h3>
<div class="blog-date"><?php print format_date($now, 'custom', 'M j Y g:i a', 'America/New_York'); ?></div>
<!-- <div class="blog-body"><?php print check_plain($blog_entry->body[LANGUAGE_NONE][0]['value']); ?></div> -->
<div class="blog-body"><?php print render($trimmed['body']); ?></div>
<div class="blog-read-more"><a href="<?php print $base_url; ?>/node/<?php print $blog_entry->nid; ?>">...read more</a></div>
