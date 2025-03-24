#!/bin/bash


ssh-keyscan -H 10.10.0.1 >> ~/.ssh/known_hosts
scp artifacts/s21_cat artifacts/s21_grep server@10.10.0.1:/usr/local/bin
ssh server@10.10.0.1 "chmod +x /usr/local/bin/s21_cat /usr/local/bin/s21_grep"
