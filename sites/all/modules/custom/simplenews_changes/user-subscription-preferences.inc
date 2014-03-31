<?php

function simplenews_save_user_preferences_page() {
  if (empty($_REQUEST['mail']) || empty($_REQUEST['hash_code'])) {
    return;
  }

  $mail = $_REQUEST['mail'];

  $uid = 0;
  if ($account = user_load_by_mail($mail)) {
    $uid = $account->uid;
  }
  $hash_string = md5($mail . '-' . $uid);

  if ($hash_string != $_REQUEST['hash_code']) {
    return;
  }

  $form = drupal_get_form('simplenews_user_mail_subscription_settings_form', $account->uid, $mail);
  $form = drupal_render($form);

  return $form;
}

function simplenews_user_mail_subscription_settings_form($form, $form_state, $uid) {
  $form['subscription_type'] = array(
    '#type' => 'radios',
    '#options' => array('daily' => t('Daily'), 'weekly' => t('Weekly')),
    '#title' => t('How frequently do you want to receive email about new blog post?'),
    '#default_value' => 'daily'
  );

  $form['uid'] = array(
    '#type' => 'hidden',
    '#default_value' => $uid
  );

  $form['mail'] = array(
    '#type' => 'hidden',
    '#default_value' => $mail
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function simplenews_user_mail_subscription_settings_form_submit($form, $form_state) {

  $values = (object) $form_state['values'];
  $snid = simplenews_changes_get_subscriber_id_using_mail($values->mail);

  db_merge('simplenews_changes_user_preferences')
    ->key(array('snid' => $snid))
    ->fields(array(
      'weekly' => $values->weekly_update,
    ))
    ->execute();

  drupal_set_message("Your configuration has been saved.");
}
