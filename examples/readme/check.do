namespace main {
    task will-run {
        check {
            exit 1
        }
        cmds {
            echo "this will run, because check will fail"
        }
    }

    task will-not-run {
        check {
            exit 0
        }
        cmds {
            echo "this will not run, because check succeeds"
        }
    }
}
