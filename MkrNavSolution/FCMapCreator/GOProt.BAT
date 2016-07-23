@ECHO OFF
protoc --java_out=./src ./src/flcnav_odb.proto
::protoc --java_out=./src ./src/fileformat.proto
::protoc --java_out=./src ./src/osmformat.proto

