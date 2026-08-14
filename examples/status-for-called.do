namespace main {
    task main {
        vars {
            mainvars="mainvars"
        }
        check {
            echo "$mainvars"
            echo "$testvars"
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
