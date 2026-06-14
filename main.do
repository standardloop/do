include "example.do"

namespace foo {

    flags {
        
    }

    vars {
        CC = "gcc"
        SOURCE_FILES = ["main.c"]
    }

    env {

    }

    task test {
        echo $x
        echo $y
    }

    task foo {
        description = "hello"
        flags {

        }
        deps {
            task test("foo", "bar")
        }
        cmds {
            echo "hello!"
        }
        status {

        }
    }
}
