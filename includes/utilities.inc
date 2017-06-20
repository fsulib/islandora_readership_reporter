<?php

/**
 * @file
 * Utitlity functions.
 */

function islandora_readership_reporter_run_readership_report() {
  $recipients = islandora_readership_reporter_get_all_person_entities();
  foreach ($recipients as $recipient) {
    watchdog('irr', json_encode($recipient));
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

    /*
    $person_object = islandora_object_load($result['object']['value']);
    if ($person_object['MADS']) {
      $person_metadata = simplexml_load_string($person_object['MADS']->content);

      if (isset($person_metadata->affiliation->email) && $person_metadata->affiliation->email != '') {

        $person = array();
        $person['pid'] = $person_object->id;
        $person['name'] = (string) $person_metadata->authority->titleInfo->title;
        $person['email'] = (string) $person_metadata->affiliation->email;
        $recipients[] = $person;
      }   
    }   
  }
  
  return $recipients;
}
*/

function islandora_readership_reporter_extract_person_data_via_pid($person_pid) {
  $person_object = islandora_object_load($person_pid);
  if ($person_object['MADS']) {
    $person_metadata = simplexml_load_string($person_object['MADS']->content);
    $person_email = 
  
    // 1st layer of filtration against invalid person entities: lacking a valid email
    if (isset($person_metadata->affiliation->email) && $person_metadata->affiliation->email != '') {

    }

  }
}

function islandora_readership_reporter_extract_person_identifier($person_metadata) {
}


function islandora_readership_reporter_get_citation_pids($person) {

        $identifiers = $person_metadata->identifier;
        foreach ($identifiers as $identifier) {
          $string_id = (string) $identifier;
          if ($identifier['type'] == 'u1' && !empty($string_id)) {
            $results = islandora_entities_get_related_pids($string_id, (string) $simplexml->authority->titleInfo->title, $type);
          }
        }




  return array();
}

function islandora_readership_reporter_build_readership_report($person) {
  // Get all PIDs for a person via islandora_entities_get_related_pids
  // Get usage stats for each PID via islandora_readership_reporter_get_pid_stats
  // Return an HTML email with HTML tables for each PID
}

function islandora_readership_reporter_get_pid_stats($pid) {
  // Get usage stats for PID
  // return as array('current' => $x, 'all' => $y);
}

