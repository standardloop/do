namespace main {
    task main {
        cmds {
            echo "hello!"
        }
    }
    task will-not-run {
        check {
            exit 0 # since this exits 0, the cmds won't run
        }
        cmds {
            echo "this won't run!"
        }
    }
    task run-before {
        cmds {
            echo "run-before"
        }
    }
    task run-after {
        cmds {
            run-before
            echo "run-after"
        }
    }
    task does-not-exist {
        cmds {
            dont-have
        }
    }
}
