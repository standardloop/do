namespace main {
    task main {
        cmds {
            x="hello"
            echo $x
        }
    }
    task test {
        cmds {
            x="test"
            echo $x
        }
    }
    task build {
        cmds {
            gcc -Werror -Wextra -Wall -Wfree-nonheap-object -std=c17 \
                main.c  lexer.c  parser.c  do.c  dynamicarray.c  task.c  namespace.c  \
                -L/usr/local/lib/standardloop \
                -pthread -lstandardloop-logger -lstandardloop-util -lstandardloop-json \
                -o do-from-do
        }
    }
}
