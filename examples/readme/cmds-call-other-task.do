namespace main {
    task call-me {
        cmds {
            echo "call-me task"
            echo $example
        }
    }
    task main {
        vars {
            example="defined in main, but called tasks can see"
        }
        cmds {
            call-me
        }
    }
}
