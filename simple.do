namespace main {
    task hello {
        cmds {
            echo "hello!"
        }
    }
    task main {
        cmds {
            printf "main\n"
        }
    }
}
