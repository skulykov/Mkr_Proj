@ECHO OFF
protoc_300 --java_out=./src ./my_common_proto_comm_lite.proto

protoc_300 --cpp_out=./src ./my_common_proto_comm_lite.proto
protoc_300 --cpp_out=./src ./my_proto_comm_lite.proto

protoc_300 --csharp_out=./src ./my_csharp_proto3_comm.proto
::protoc_300 --csharp_out=./src ./addressbook.proto

::protoc_300  --js_out=./src/JS ./my_csharp_proto3_comm.proto