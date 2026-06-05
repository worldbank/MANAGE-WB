#!/bin/bash
cd -- "$(dirname "$BASH_SOURCE")"
java -Xmx2G -Xverify:none -XX:+UseParallelGC -XX:PermSize=20M -XX:MaxNewSize=32M -XX:NewSize=32M -Djava.library.path=jars -jar jars/gig.jar ./manage.ini /manage.xml
