include "example.do"

namespace foo {

    vars {
        CC = "gcc"
        SOURCE_FILES = ["main.c"]
    }

    env {

    }

    task test(x, y) {
        echo $x
        echo $y
    }

    task foo() {
        flags {
           
        }
        deps {
            task test("foo", "bar")
        }
        cmds {

        }
        status {

        }
    }
}
