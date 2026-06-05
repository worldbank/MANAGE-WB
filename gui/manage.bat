set PATH=jars;
..\jre-8.0.212.03-hotspot\bin\java -Dfile.encoding=UTF-8 -Duser.language=en -Duser.country=US -Xmx1G -Xverify:none -XX:+UseParallelGC -XX:MaxHeapSize=2G -XX:MaxNewSize=32M -XX:NewSize=32M -Djava.library.path=jars  -jar jars\gig.jar manage.ini manage.xml
