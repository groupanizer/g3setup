<?php

function g3setup_main_page() {

  drupal_add_css(drupal_get_path('module', 'g3setup') . '/g3setup.css');

  $ra = array();
  $status = g3setup_status();

  if( empty($status) ) return t("There are no setup hooks defined");

  // let's send these links back to setup if we can
  $dest = drupal_get_destination();

  $header = array(t('Status'), t('Description'));

  $done_cnt = 0;
  foreach($status AS $s) {
    // status
    if( !isset($s['status'] ) ) $s['status'] = 'pending';
    if( $s['status'] == 'done' ) {
      $status_icon = "<i class='fa fa-check-circle done'></i>";
      $done_cnt++;
    } else {
      $status_icon = "<i class='fa fa-circle pending'></i>";
    }

    // name
    $name = isset($s['link']) ? l($s['name'], $s['link'], array('query'=>$dest)) : $s['name'];
    $rows[] = array(
      array(
        'data' => $status_icon,
        'class' => array('status-icon'),
      ),
      $name . '<br>' . $s['description'],
    );
  }

  // progress message
  $progress_message = t("You have completed @done of @total steps. Click on the step names below to configure each section.", array('@done'=>$done_cnt, '@total'=>sizeof($status)));
  if( $done_cnt >= sizeof($status) ) {
    $progress_message .= " " . t("Nicely done!");
  }
  $ra['progress'] = array(
    '#markup' => $progress_message,
  );

  $ra['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  $action_buttons = drupal_get_form('g3setup_actions');
  $ra['buttons'] = array(
    '#markup' => drupal_render($action_buttons),
  );

  return $ra;
}

function g3setup_actions() {
  $form['hide'] = array(
    '#type' => 'submit',
    '#value' => "Hide the setup progress bar",
  );

  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => "Reset all steps and show progress bar",
  );

  return $form;
}

function g3setup_actions_submit($form, $form_state) {

  $btn = $form_state['clicked_button']['#id'];

  // reset all setup steps by deleting saved state
  if( $btn == 'edit-reset' ) {
    $variable_names = db_query("SELECT name FROM variable WHERE name LIKE 'g3setup%'")->fetchCol();
    foreach( $variable_names AS $name ) {
      variable_del($name);
    }
  }

  // hide the setup progress bar
  if( $btn == 'edit-hide' ) {
    variable_set('g3setup_hide_progress', TRUE);
  }
}
