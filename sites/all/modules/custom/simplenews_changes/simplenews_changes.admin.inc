<?php
/**
 * Menu callback: subscription administration.
 */
function simplenews_changes_admin_subscription() {

  // Since this custom code will use existing function from simplenews inc
  module_load_include('inc', 'simplenews', 'includes/simplenews.admin');

  $form['filter'] = simplenews_subscription_filter_form();
  $form['#submit'][] = 'simplenews_subscription_filter_form_submit';
  $form['filter']['#theme'] = 'simplenews_filter_form';
  $form['admin'] = simplenews_changes_subscription_list_form();

  return $form;
}

/**
 * Menu callback: subscription administration.
 *
 * @see simplenews_subscription_list_form_validate()
 * @see simplenews_subscription_list_form_submit()
 *
 * @todo Subscriber maintenance needs overhaul now we have more data available.
 */
function simplenews_changes_subscription_list_form() {
  // Build an 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $options = array();
  foreach (module_invoke_all('simplenews_subscription_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => 'activate',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('simplenews_subscription_list_form_submit'),
    '#validate' => array('simplenews_subscription_list_form_validate'),
  );

  // Table header. Used as tablesort default
  $header = array(
    'mail' => array('data' => t('Email'), 'field' => 'sn.mail', 'sort' => 'asc'),
    'username' => array('data' => t('Username'), 'field' => 'u.name'),
    'status' => array('data' => t('Status'), 'field' => 'sn.activated'),
    'language' => array('data' => t('Language'), 'field' => 'sn.language'),
    'created' => array('data' => t('Created'), 'field' => 'sn.created'),
    'weekly' => array('data' => t('Weekly'), 'field' => 'p.weekly'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('simplenews_subscriber', 'sn')->extend('PagerDefault')->extend('TableSort');
  simplenews_build_subscription_filter_query($query);
  $query->leftJoin('users', 'u', 'sn.uid = u.uid');
  $query->leftJoin('simplenews_changes_user_preferences', 'p', 'sn.snid = p.snid');
  $query->innerJoin('simplenews_subscription', 'su', 'sn.snid = su.snid');
  $query->condition('su.status', SIMPLENEWS_SUBSCRIPTION_STATUS_SUBSCRIBED);
  $query->addField('u', 'name', 'name');
  $query->addField('p', 'weekly', 'weekly');

  // Add query tag so that other module can apply for query_alter hook
  $query->addTag('simplenews_changes_subscription');

  $result = $query
    ->fields('sn', array('snid', 'activated', 'mail', 'uid', 'language', 'created'))
    ->limit(30)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();

  foreach ($result as $subscriber) {
    $options[$subscriber->snid] = array(
      'mail' => check_plain($subscriber->mail),
      'username' => isset($subscriber->uid) ? l($subscriber->name, 'user/' . $subscriber->uid) : check_plain($subscriber->name),
      'status' => theme('simplenews_status', array('source' => 'activated', 'status' => $subscriber->activated)),
      'language' => check_plain($subscriber->language),
      'created' => format_date($subscriber->created, 'small'),
      'weekly' => !empty($subscriber->weekly) ? 'Yes' : 'No',
      'operations' => l(t('edit'), 'admin/people/simplenews/users/edit/' . $subscriber->snid, array(), $destination),
    );
  }

  $form['subscribers'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No subscribers available.'),
  );

  $form['pager'] = array(
    // Calling theme('pager') directly so that it the first call after the
    // pager query executed above.
    '#markup' => theme('pager'),
  );

  return $form;
}
