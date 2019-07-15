import os

env = Environment()

root_path = os.getcwd()

env.Append(CPPPATH = [root_path,
        '/home/zhangjun/pb/include',
        ])
env.Append(LIBPATH = ['/home/zhangjun/pb/lib'])

env.Append(LIBS = ['boost_system'])
env.Append(LIBS = ['pthread'])
env.Append(LIBS = ['protobuf'])

#env.Append(CCFLAGS = ['-Wall', '-O2', '-std=c++11', '-g'])
env.Append(CCFLAGS = ['-O2', '-std=c++11', '-g', '-DUSE_RTTI=1'])
env.Append(LINKLAGS = ['-static'])

# https://github.com/SCons/scons/wiki/ProtocBuilder
protoc = Builder(action="protoc --cpp_out=. $SOURCE")
env2 = Environment(
        ENV = {'PATH': os.environ['PATH']},
        BUILDERS={'Protoc':protoc})
env2.Protoc("echo_service.pb.cc echo_service.pb.h".split(), "echo_service.proto")

env.Program(
    target = "main",
    source = [
        "main.cc",
        "echo_task.cc",
        "recog_engine.cc",
        "socket_util.cc",
        "echo_service.pb.cc",
    ],
)
