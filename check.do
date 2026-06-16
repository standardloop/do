namespace main {
    task main {
        check {
            exit 1;
        }
        cmds {
            echo "inside other task"
        }
    }
}
