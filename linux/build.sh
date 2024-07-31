#!/bin/bash

ABSOLUTE_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname $ABSOLUTE_PATH)"

echo $ABSOLUTE_PATH
source "$ABSOLUTE_PATH/version.txt"
source "$ABSOLUTE_PATH/helper_functions.sh"
source "$ABSOLUTE_PATH/action_functions.sh"

TARGET_CHOICES="all strext jal"
ACTION_CHOICES="build make_release clean setup test"
DEFAULT_TARGET="all"
DEFAULT_ACTION="build"

export STR_EXT_VER
export JAL_VER

function printUsage() {
    local targetsWithDefault="${TARGET_CHOICES/$DEFAULT_TARGET/$DEFAULT_TARGET(default)}"
    local actionsWithDefault="${ACTION_CHOICES/$DEFAULT_ACTION/$DEFAULT_ACTION(default)}"
    echo -e \
"./build.sh [target] [action] \n\
target: ${targetsWithDefault// /, } \n\
action: ${actionsWithDefault// /, } \n\
 \n\
JAL_VER: $JAL_VER\n\
STR_EXT_VER: $STR_EXT_VER\n\
"
    return 1
}

function checkParams() {
    local cnt=$1
    local target=$2
    local action=$3
    ACTION_CHOICE=$DEFAULT_ACTION
    TARGET_CHOICE=$DEFAULT_TARGET

    notValid=0
    if [[ $cnt -gt 2 ]] ; then
        echo "Err: too many args."
        notValid=1
    #help flag is an "invalid" build command
    elif [[ $cnt -gt 0 ]] ; then
        local args=$@
        inList "${args[@]}" "--help" || inList "${args[@]}" "-h"
        if [[ $? -eq 0 ]] ; then
            notValid=1
        else 
            inList "${TARGET_CHOICES[@]}" $target 
            if [[ $? -ne 0 ]] ; then
                echo "Err: not valid target [$target]"
                notValid=1
            elif [[ $cnt -eq 2 ]] ; then
                inList "${ACTION_CHOICES[@]}" $action
                if [[ $? -ne 0 ]] ; then
                    echo "Err: not valid action [$action]"
                    notValid=1
                fi
            fi
        fi
    fi

    return $notValid
}




###########################
# START OF SCRIPT
###########################
target=$DEFAULT_TARGET
action=$DEFAULT_ACTION
status=1
checkParams $# $1 $2
if [[ $? -ne 0 ]] ; then
    printUsage
#if valid, fill in defualts
else
    if [[ $# -gt 0 ]] ; then
        target=$1
        if [[ $# -eq 2 ]] ; then
            action=$2
        fi
    fi
    #use targets as a way to handle 'all' case
    targets=($target)
    if [[ $targets == "all" ]] ; then
        #remove all, only need individual choices
        targets=("${TARGET_CHOICES/all /}")
    fi
    for target in ${targets} ; do
        if [[ "$action" == "setup" ]] ; then
            setupBuild $target
        elif [[ "$action" == "clean" ]] ; then
            wipeBuild $target
        elif [[ "$action" == "build" ]] ; then
            buildTarget $target
        elif [[ "$action" == "test" ]] ; then
            testBuild $target
        elif [[ "$action" == "make_release" ]] ; then
            createRelease $target
        else
            printUsage
        fi
        status=$?
    done
fi

if [[ status -eq 0 ]] ; then
    echo "COMPLETED"
else
    echo "FAILED"
fi

exit $status
