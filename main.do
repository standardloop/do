namespace main {
    vars {
        CC="clang"
        CC_FLAGS="-g -Werror -Wextra -Wall -Wfree-nonheap-object -std=c17"
        EXECUTABLE_NAME="do-task"
        DYN_LIBS_USED_PATH="-L/usr/local/lib/standardloop"
        DYN_LIBS_USED="-lstandardloop-logger -lstandardloop-util"
    }

    task hi {
        check {
            exit 0
        }
        cmds {
            echo "hi!"
        }
    }

    task download-dependency {
        check {
            set -e
            otool -L /usr/local/lib/standardloop/libstandardloop-$DYLIB_NAME.dylib | head -n 2 | tail -n 1 | grep "current version $DYLIB_VERSION" >/dev/null 2>&1
            DYLIB_NAME_UPPER_CASE=$(echo "$DYLIB_NAME" | tr '[:lower:]' '[:upper:]')
            cat /usr/local/include/standardloop/$DYLIB_NAME.h | grep "STANDARDLOOP_${DYLIB_NAME_UPPER_CASE} \"$DYLIB_VERSION\""
        }
        cmds {
            set -e
            mkdir -p dependencies/tmp-$DYLIB_NAME
            cd dependencies/tmp-$DYLIB_NAME
            curl -O -J -L https://github.com/standardloop/$REPO_NAME/releases/download/v$DYLIB_VERSION/libstandardloop-$DYLIB_NAME.zip
            unzip libstandardloop-$DYLIB_NAME.zip
            sudo mkdir -p /usr/local/lib/standardloop/
            sudo mkdir -p /usr/local/include/standardloop/
            sudo mv libstandardloop-$DYLIB_NAME.dylib /usr/local/lib/standardloop/
            sudo mv $DYLIB_NAME.h /usr/local/include/standardloop/ && rm libstandardloop-$DYLIB_NAME.zip
            cd ../
        }
    }

    task check-vars {
        check {
            set -e
            otool -L /usr/local/lib/standardloop/libstandardloop-$DYLIB_NAME.dylib | head -n 2 | tail -n 1 | grep "current version $DYLIB_VERSION" >/dev/null 2>&1
            DYLIB_NAME_UPPER_CASE=$(echo "$DYLIB_NAME" | tr '[:lower:]' '[:upper:]')
            cat /usr/local/include/standardloop/$DYLIB_NAME.h | grep "STANDARDLOOP_${DYLIB_NAME_UPPER_CASE} \"$DYLIB_VERSION\""
        }
        cmds {
            set -e
            mkdir -p dependencies/tmp-$DYLIB_NAME
            cd dependencies/tmp-$DYLIB_NAME
            curl -O -J -L https://github.com/standardloop/$REPO_NAME/releases/download/v$DYLIB_VERSION/libstandardloop-$DYLIB_NAME.zip
            unzip libstandardloop-$DYLIB_NAME.zip
            sudo mkdir -p /usr/local/lib/standardloop/
            sudo mkdir -p /usr/local/include/standardloop/
            sudo mv libstandardloop-$DYLIB_NAME.dylib /usr/local/lib/standardloop/
            sudo mv $DYLIB_NAME.h /usr/local/include/standardloop/ && rm libstandardloop-$DYLIB_NAME.zip
            cd ../
        }
    }

    task echo-vars {
        check {
            echo "$REPO_NAME"
            echo "$DYLIB_VERSION"
            echo "$DYLIB_NAME"
        }
        cmds {
            echo "$REPO_NAME"
            echo "$DYLIB_VERSION"
            echo "$DYLIB_NAME"
        }
    }

    task logger-test {
        vars {
            REPO_NAME="c-logger"
            DYLIB_VERSION="0.0.17"
            DYLIB_NAME="logger"
        }
        cmds {
            echo-vars
        }
    }

    task dependencies {
        cmds {
            logger
            util
        }
    }

    task logger {
        vars {
            REPO_NAME="c-logger"
            DYLIB_VERSION="0.0.17"
            DYLIB_NAME="logger"
        }
        cmds {
            download-dependency
        }
    }
    
    task util {
        vars {
            REPO_NAME="c-util"
            DYLIB_VERSION="0.0.8"
            DYLIB_NAME="util"
        }
        cmds {
            download-dependency
        }
    }

    task main {
        cmds {
            set -e
            dependencies
            $CC $CC_FLAGS \
                main.c lexer.c parser.c do.c dynamicarray.c task.c namespace.c \
                $DYN_LIBS_USED_PATH $DYN_LIBS_USED \
                -o build/$EXECUTABLE_NAME
        }
    }

    task release {
        cmds {
            sudo cp $EXECUTABLE_NAME /usr/local/bin/
        }
    }

    task clean {
        cmds {
            rm $EXECUTABLE_NAME
        }
    }
}
