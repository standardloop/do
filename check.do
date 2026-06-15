namespace main {
    task main {
        check {
            exit 0
        }
        cmds {
            echo "inside other task"
        }
    }
}
