@ECHO OFF
::protoc --java_out=./src ./src/flcnav_odb.proto
protoc --cpp_out=./ ./fileformat.proto
protoc --cpp_out=./ ./osmformat.proto

