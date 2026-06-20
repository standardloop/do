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
            exit 0;
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
    task base {
        cmds {
            echo "base"
        }
    }
    task basetwo {
        cmds {
            base
            base
        }
    }
    task runother {
        cmds {
            base
            basetwo
            base
            basetwo
        }
    }
}
