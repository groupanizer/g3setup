<?php

function g3setup_main_page() {

  drupal_add_css(drupal_get_path('module', 'g3setup') . '/g3setup.css');

  $ra = array();
  $status = g3setup_status();

  if( empty($status) ) return t("There are no setup hooks defined");

  // let's send these links back to setup if we can
  $dest = drupal_get_destination();

  $header = array(t('Status'), t('Description'), '');

  $done_cnt = 0;
  foreach($status AS $s) {

    // status icon
    if( !isset($s['status'] ) ) $s['status'] = 'pending';
    if( $s['status'] == 'done' ) {
      $status_icon = "<span class='g3setup-status-icon'><i class='fa fa-check-circle done'></i></span>";
      $done_cnt++;
    } else {
      $status_icon = "<span class='g3setup-status-icon'><i class='fa fa-circle pending'></i></span>";
    }

    // text description
    $text = '';
    $text .= "<div class='g3setup-link'>";
    $text .= isset($s['icon']) ? $s['icon'] : "<i class='fa fa-square'></i>";
    $text .= "&nbsp";
    $text .= isset($s['link']) ? l($s['name'], $s['link'], array('query'=>$dest)) : $s['name'];
    $text .= "</div>";
    $text .= "<div class='g3setup-description'>{$s['description']}</div>";
    if( isset($s['navigation']) ) {
      $text .= "<div class='g3setup-navigation'>" . t("Find this later at") . " {$s['navigation']}</div>";
    }

    // button
    $button = '';
    if( isset($s['manual-completion']) && $s['status'] != 'done' ) {
      $form = drupal_get_form($s['manual-completion']);
      $button = drupal_render($form);
    }

    $rows[] = array(
      array( 'data' => $status_icon, 'class' => array('status-icon')),
      array( 'data' => $text, 'class' => array('status-text')),
      array( 'data' => $button, 'class' => array('status-button')),
    );

  }

  // progress message
  if( sizeof($status) == $done_cnt ) {
    $progress_message = "<span class='g3setup-congrats'>" . t("Congratulations! You have completed the setup.") . "</span><br>";
    $progress_message .= t("You can hide the progress bar with the button at the bottom of this page.");
  } else {
    $progress_message = t("Click on the step names below to configure each section. Refer to our !qsg for setup recommendations for your choir type.", array('!qsg' => l("Quick start guide", 'https://support.choirgenius.com/content/quick-start-setup-guides', array('absolute'=>TRUE))));
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
