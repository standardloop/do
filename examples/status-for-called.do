namespace main {

    vars {
        global="global"
    }
    task main {
        vars {
            mainvars="mainvars"
        }
        check {
            echo "$mainvars"
            echo "$testvars"
            echo "$global"
        }
        cmds {
            echo "running task main"
        }
    }
    task test {
        vars {
            testvars="testvars"
        }
        cmds {
            main
            echo "running task test"
        }
    }
}
