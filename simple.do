namespace main {
    vars {
        CC="gcc"
        FLAGS="-Werror -Wextra -Wall -Wfree-nonheap-object -std=c17"
        SOURCES="main.c  lexer.c  parser.c  do.c  dynamicarray.c  task.c  namespace.c"
        DLIB_PATH="-L/usr/local/lib/standardloop"
        DLIBS="-lstandardloop-logger -lstandardloop-util"
        OUPUT_BINARY="do-from-do"
        x="yes"
        y="test"
    }
    task main {
        cmds {
            x="hello"
            echo $x
        }
    }
    task test {
        cmds {
            echo $x
            echo $y
        }
    }
    task call {
        cmds {
            test
            echo $x
            echo $x
            echo $x
        }
    }
    task build {
        cmds {
            $CC $FLAGS $SOURCES $DLIB_PATH $DLIBS -o $OUPUT_BINARY
        }
    }
}
