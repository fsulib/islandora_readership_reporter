<?php

/**
 * @file
 * Utitlity functions.
 */

function islandora_readership_reporter_run_readership_reports() {
  $recipients = islandora_readership_reporter_get_all_person_entities();
  foreach ($recipients as $recipient) {
    islandora_readership_reporter_run_readership_report($recipient);
  }
}

function islandora_readership_reporter_get_all_person_entities() {
  $query = <<<EOQ
SELECT ?object FROM <#ri> WHERE {
  ?object <info:fedora/fedora-system:def/model#hasModel> <info:fedora/islandora:personCModel> .
}
EOQ;

  $connection = islandora_get_tuque_connection();
  $results = $connection->repository->ri->sparqlQuery($query);
  $recipients = array();
  foreach ($results as $result) {
    $person_pid = $result['object']['value'];
    $person_data = islandora_readership_reporter_extract_person_data_via_pid($person_pid);
    if ($person_data) {
      $recipients[] = $person_data;
    }
  }
 
  return $recipients;
}

function islandora_readership_reporter_extract_person_data_via_pid($person_pid) {
  $person_object = islandora_object_load($person_pid);
  if ($person_object['MADS']) {
    $person_metadata = simplexml_load_string($person_object['MADS']->content);
    $person_email = islandora_readership_reporter_field_check($person_metadata->affiliation->email);
    $person_name = islandora_readership_reporter_field_check($person_metadata->authority->titleInfo->title);
    $person_id = islandora_readership_reporter_field_check(islandora_readership_reporter_get_u1($person_metadata));
    $person_pids = ($person_id ? islandora_readership_reporter_get_author_pids($person_id) : FALSE); 
  
    if ($person_email && $person_name && $person_id && count($person_pids) > 0) {
      $person_data = array(
        'pid' => $person_object->id,
        'email' => $person_email,
        'name' => $person_name,
        'u1' => $person_id,
        'works' => $person_pids,
      );
      return $person_data;
    }
  }
  else {
    return FALSE;
  }
}

function islandora_readership_reporter_get_u1($person) {
  $identifiers = $person->identifier;
  foreach ($identifiers as $identifier) {
    $string_id = (string) $identifier;
    if ($identifier['type'] == 'u1' && !empty($string_id)) {
      $person_id = $string_id;
    }
  }
  if (isset($person_id)) {
    return $person_id;
  }
  else {
    return FALSE;
  }
}

function islandora_readership_reporter_field_check($field) {
  $string_field = (string) $field;
  if (isset($string_field) && $string_field != '') {
    return $string_field;
  }
  else {
    return FALSE;
  }
}

function islandora_readership_reporter_get_author_pids($person_id) {
  $solr_link = 'mods_name_nameIdentifier_mt';
  $query = "+{$solr_link}:\"({$person_id})\"";
  $params = array('fl' => 'PID');
  $qp = new IslandoraSolrQueryProcessor();
  $qp->buildAndExecuteQuery($query, $params);
  try {
    $search_results = $qp->islandoraSolrResult['response']['objects'];
  }
  catch (Exception $e) {
    watchdog_exception('Islandora Readership Reporter', $e, 'Got an exception while searching entities for callback.', array(), WATCHDOG_ERROR);
    $search_results = array();
  }

  $results = array();
  if ($search_results) {
    foreach ($search_results as $choice) {
      $results[$choice['PID']] = array(
        'PID' => $choice['PID'],
        'title' => $choice['object_label'],
      );
    }
  }
  $pids = array();
  foreach ($results as $result) {
    $pids[] = array('pid' => $result['PID'], 'title' => $result['title']);
  }
  return $pids;
}

function islandora_readership_reporter_run_readership_report($person_data) {

  $now = time();
  $last_month = date('F', strtotime("-1 month"));
  $last_month_year = date('Y', strtotime("-1 month"));
  $subject = "{$last_month} Readership Report";

  $number_of_papers = count($person_data['works']);
  $paper_word = ($number_of_papers > 1 ? "papers" : "paper"); 

  $primary_color = "#5E0117";
  $secondary_color = "#CEC194";

  $works = array();
  $user_monthly_views = 0;
  $user_monthly_downloads = 0;
  $user_total_views = 0;
  $user_total_downloads = 0; 
 
  
  foreach ($person_data['works'] as $work) {
    $stats = islandora_readership_reporter_get_pid_stats($work);
    if (!empty($stats)) { 
      $user_monthly_views += $stats['last_month_views'];
      $user_monthly_downloads += $stats['last_month_downloads'];
      $user_total_views += $stats['total_views'];
      $user_total_downloads += $stats['total_downloads']; 
      $works[] = $stats; 
    }
  }
  

  $msg = <<<MSG
<!doctype html>
<html>
<head>
  <title>{$subject}</title>
</head>
<body>
<img src="https://www.lib.fsu.edu/sites/default/files/pictures/fsul-header.png" style="width:100%;"/>
<p>Greetings {$person_data['name']},</p>
<p>Your {$number_of_papers} {$paper_word} in the <a href="http://diginole.lib.fsu.edu/repository">FSU Research Repository</a> attracted {$user_monthly_views} views and {$user_monthly_downloads} downloads in {$last_month}, for a total of {$user_total_views} views and {$user_total_downloads} downloads overall.</p>
MSG;

  // foreach works as work
  foreach ($works as $work) {
    $msg .= <<<MSG
<br/>
<p><a style="color:{$primary_color};" href="http://diginole.lib.fsu.edu/islandora/object/{$work['pid']}">{$work['title']}</a></p>
<p>This record has received:</p><br/>
<table style="border-collapse: collapse;">
  <tr>
    <th></th>
    <th style="border:1px solid black;padding:5px 10px;background-color:{$primary_color};color:{$secondary_color};"><strong>{$last_month} {$last_month_year}</strong></th>
    <th style="border:1px solid black;padding:5px 10px;background-color:{$primary_color};color:{$secondary_color};"><strong>All-Time</strong></th>    
  </tr>
  <tr>
    <th style="border:1px solid black;padding:5px 10px;background-color:{$primary_color};color:{$secondary_color};"><strong>Views</strong></th>
    <td style="border:1px solid black;padding:5px 10px;text-align:center;"><strong>{$work['last_month_views']}</strong></td>    
    <td style="border:1px solid black;padding:5px 10px;text-align:center;"><strong>{$work['total_views']}</strong></td>    
  </tr>
  <tr>
    <th style="border:1px solid black;padding:5px 10px;background-color:{$primary_color};color:{$secondary_color};"><strong>Downloads</strong></th>
    <td style="border:1px solid black;padding:5px 10px;text-align:center;"><strong>{$work['last_month_downloads']}</strong></td>    
    <td style="border:1px solid black;padding:5px 10px;text-align:center;"><strong>{$work['total_downloads']}</strong></td>    
  </tr>
</table><br/><br/>
MSG;
  }

  $msg .= <<<MSG
<p>If you have questions, comments or feedback regarding DigiNole, the FSU Research Repository or this readership report, please <a style="color:#5E0117;" href="http://lib.fsu.edu/drs/repository/contact">let us know</a>.</p>
<p>Best regards,<br/>
DigiNole Support Team<br/>
<a href="mailto:lib-support@fsu.edu">lib-support@fsu.edu</a><br/>
(850) 645-2600</p>
</body>
</html>
MSG;

  $params = array();
  $params['subject'] = $subject;
  $params['body'] = $msg;

  drupal_mail('islandora_readership_reporter', 'report', 'bjbrown@fsu.edu', language_default(), $params, 'no-reply@islandorad.fcla.edu');
}

function islandora_readership_reporter_get_pid_stats($pid) {

  global $base_url;
  $time_last_month = strtotime('-1 month', time());

  $stats = json_decode(file_get_contents("{$base_url}/islandora_usage_stats_callbacks/object_stats/{$pid['pid']}"), TRUE);
  if (!isset($stats['error'])) {
    $total_views = $stats['legacy-views'] + count($stats['views']);
    $total_downloads = $stats['legacy-downloads'] + count($stats['downloads']);
    $last_month_views = 0;
    foreach ($stats['views'] as $view) {
      $last_month_views += ($view['time'] > $time_last_month ? 1 : 0);
    }
    $last_month_downloads = 0;
    foreach ($stats['downloads'] as $download) {
      $last_month_downloads += ($download['time'] > $time_last_month ? 1 : 0);
    }


    $counts = array(
      'pid' => $pid['pid'], 
      'title' => $pid['title'],
      'total_views' => $total_views, 
      'total_downloads' => $total_downloads,
      'last_month_views' => $last_month_views,
      'last_month_downloads' => $last_month_downloads,
    );
  }
  else {
    $counts = FALSE;
  }

  return $counts;
}
