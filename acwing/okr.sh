#!/bin/bash


git log --since=midnight --name-only --pretty=format:'' | grep -v '^$' | sort |uniq


cnt=`git log --since=midnight --name-only --pretty=format:'' | grep -v '^$' |grep '.cc$' |sort |uniq | wc -l`
echo "today you finished $cnt acwing in cpp, happy coding ry!"

#git log --since='7 days ago' --name-only --pretty=format:'' | grep -v '^$' |grep '.rs$'


cnt=`git log --since='7 days ago' --name-only --pretty=format:'' | grep -v '^$' |grep '.cc$' |sort|uniq| wc -l`
echo "last 7 days you finished $cnt acwing in cpp, happy coding ry!"
