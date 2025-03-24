#!/bin/bash

TELEGRAM_BOT_TOKEN="${TELEGRAM_BOT_TOKEN}"
TELEGRAM_USER_ID="${TELEGRAM_USER_ID}"

URL="https://api.telegram.org/bot$TELEGRAM_BOT_TOKEN/sendMessage"

if [[ $CI_JOB_STATUS == "failed" ]]; then
  var_status="❎"
else
  var_status="✅"
fi

TEXT="$CI_PROJECT_NAME%0AURL: $CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/%0ABranch: $CI_COMMIT_REF_SLUG %0A%0AStage: $CI_JOB_STAGE, Status: $CI_JOB_STATUS $var_status"

curl -s -d "chat_id=$TELEGRAM_USER_ID&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null

