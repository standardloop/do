namespace main {
    task run {
        check {
            exit 1;
        }
        cmds {
            echo "this task will run"
        }
    }
    task norun {
        check {
            echo "norun check"
        }
        cmds {
            echo "this task will not run"
        }
    }
    task checkcallsnorun {
        check {
            norun
        }
        cmds {
            echo "what will happen?"
        }
    }
    task baseone {
        cmds {
            echo "base"
        }
    }
    task basetwo {
        cmds {
            baseone
            baseone
        }
    }
    task runother {
        cmds {
            norun
            norun
            norun
            norun
        }
    }
}
