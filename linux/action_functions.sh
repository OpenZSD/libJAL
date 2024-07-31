ABSOLUTE_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname $ABSOLUTE_PATH)"

source "$ABSOLUTE_PATH/helper_functions.sh"
source "$ABSOLUTE_PATH/version.txt"

#file locations
##strext
STREXT_DIR="$PROJECT_ROOT/lib/libStrExtUtil"
STREXT_BUILD_DIR="$STREXT_DIR/build"
STREXT_RELEASE_DIR_NAME="libstrext-$STR_EXT_VER"
STREXT_BUILD_RELEASE_DIR="$STREXT_DIR/build/release/$STREXT_RELEASE_DIR_NAME"
STREXT_MAKEFILE_PATH="$STREXT_BUILD_DIR/Makefile"
STREXT_TEST_DIR="$PROJECT_ROOT/tests/libStrExtUtil"
##jal
JAL_DIR="$PROJECT_ROOT/lib/libJAL"
JAL_BUILD_DIR="$JAL_DIR/build"
JAL_RELEASE_DIR_NAME="libjal-$JAL_VER"
JAL_BUILD_RELEASE_DIR="$JAL_DIR/build/release/$JAL_RELEASE_DIR_NAME"
JAL_MAKEFILE_PATH="$JAL_BUILD_DIR/Makefile"
JAL_TEST_DIR="$PROJECT_ROOT/tests/libJAL"

function checkStup() {
    local setupNotFound=1
    if [[ "$1" == "strext" ]] ; then
        if [[ -f $STREXT_MAKEFILE_PATH ]] ; then
            setupNotFound=0
        fi
    elif [[ "$1" == "jal" ]] ; then
        if [[ -f $JAL_MAKEFILE_PATH ]] ; then
            setupNotFound=0
        fi
    fi 
    return $setupNotFound
}

function checkBuild() {
    local buildNotFound=1
    return $setupNotFound
}

function setupBuild() {
    local fstat=1
    if [[ "$1" == "strext" ]] ; then
        mkdir "$STREXT_BUILD_DIR"
        mkdir "$STREXT_BUILD_DIR/include"
        cmake -S "$STREXT_DIR" -B "$STREXT_BUILD_DIR"
        fstat=$?
    elif [[ "$1" == "jal" ]] ; then
        mkdir "$JAL_BUILD_DIR"
        mkdir "$JAL_BUILD_DIR/include"
        mkdir -p "$JAL_BUILD_DIR/res/strExt"
        mkdir -p "$JAL_BUILD_DIR/res/lib"
        cmake -S "$JAL_DIR" -B "$JAL_BUILD_DIR"
        fstat=$?
    fi
    return $fstat
}

function wipeBuild() {
    local fstat=1
    if [[ "$1" == "strext" ]] ; then
        rm -rf "$STREXT_BUILD_DIR"
        mkdir "$STREXT_BUILD_DIR"
        setupBuild $1
        fstat=$?
    elif [[ "$1" == "jal" ]] ; then
        rm -rf "$JAL_BUILD_DIR"
        mkdir "$JAL_BUILD_DIR"
        setupBuild $1
        fstat=$?
    fi
    return $fstat
}

function buildTarget() {
    local fstat=1
    local curDir=$(pwd)

    checkStup $1
    fstat=$?
    if [[ $fstat -ne 0 ]]  ; then
        setupBuild $1
        fstat=$?
    fi

    if [[ $fstat -eq 0 ]]  ; then
        if [[ "$1" == "strext" ]] ; then
            cp -r "$STREXT_DIR/include" "$STREXT_BUILD_DIR"
            cd $STREXT_BUILD_DIR
            make
            fstat=$?
        elif [[ "$1" == "jal" ]] ; then
            #make sure strext is current and working
            testBuild strext
            cp -r "$JAL_DIR/include" "$JAL_BUILD_DIR"
            cp -r "$STREXT_BUILD_DIR/include/." "$JAL_BUILD_DIR/res/strExt/"
            cp -r "$STREXT_BUILD_DIR/lib_static/." "$JAL_BUILD_DIR/res/lib/"
            cd $JAL_BUILD_DIR
            make
            fstat=$?
        fi
    else
        >&2 echo "ERR: could not setup build for $1"
    fi
    cd $curDir
    return $fstat
}

function testBuild() {
    local fstat=1
    local curDir=$(pwd)

    buildTarget $1

    if [[ $? -eq 0 ]]  ; then
        if [[ "$1" == "strext" ]] ; then
            rm -rf "$STREXT_TEST_DIR/out"
            mkdir "$STREXT_TEST_DIR/out"
            mkdir "$STREXT_TEST_DIR/out/strExt"
            cp -r "$STREXT_BUILD_DIR/include/." "$STREXT_TEST_DIR/out/strExt/"
            cp -r "$STREXT_BUILD_DIR/lib_static/." "$STREXT_TEST_DIR/out/lib"
            cmake -S "$STREXT_TEST_DIR" -B "$STREXT_TEST_DIR/out"
            cd "$STREXT_TEST_DIR/out"
            make
            fstat=$?
            if [[ $fstat -eq 0 ]] ; then
                chmod 700 $STREXT_TEST_DIR/out/test
                $STREXT_TEST_DIR/out/test
                fstat=$?
            else
                >&2 echo "ERR: could not build tests for $1"
            fi
        elif [[ "$1" == "jal" ]] ; then
            rm -rf "$JAL_TEST_DIR/out"
            mkdir "$JAL_TEST_DIR/out"
            mkdir "$JAL_TEST_DIR/out/jal"
            cp -r "$JAL_BUILD_DIR/include/." "$JAL_TEST_DIR/out/jal/"
            cp -r "$JAL_BUILD_DIR/lib_static/." "$JAL_TEST_DIR/out/lib"
            cmake -S "$JAL_TEST_DIR" -B "$JAL_TEST_DIR/out"
            cd "$JAL_TEST_DIR/out"
            make
            fstat=$?
            if [[ $fstat -eq 0 ]] ; then
                chmod 700 $JAL_TEST_DIR/out/test
                $JAL_TEST_DIR/out/test
                fstat=$?
            else
                >&2 echo "ERR: could not build tests for $1"
            fi
        fi
    else
        >&2 echo "ERR: could not build $1"
    fi

    cd $curDir
    return $fstat
}

function createRelease() {
    local curDir=$(pwd)

    local libBasePath="N/A"
    local includeBasePath="N/A"
    local sysArch=$(dpkg --print-architecture)
    if [[ $sysArch -eq "amd64" ]] ; then
        libBasePath="/lib/x86_64-linux-gnu/"
        includeBasePath="/usr/include/"
    else
        >&2 echo "ERR: architecture not supported"
        return 1
    fi

    testBuild $1
    if [[ $? -eq 0 ]]  ; then
        if [[ "$1" == "strext" ]] ; then
            mkdir -p "$STREXT_BUILD_RELEASE_DIR/$libBasePath"
            mkdir -p "$STREXT_BUILD_RELEASE_DIR/$includeBasePath/strExt"
            mkdir -p "$STREXT_BUILD_RELEASE_DIR/DEBIAN"
            cp -r "$STREXT_BUILD_DIR/include/." "$STREXT_BUILD_RELEASE_DIR/$includeBasePath/strExt"
            cp -r "$STREXT_BUILD_DIR/lib_static/." "$STREXT_BUILD_RELEASE_DIR/$libBasePath" 
            cp -r "$STREXT_BUILD_DIR/lib/." "$STREXT_BUILD_RELEASE_DIR/$libBasePath" 
            printDebianCtl "libstrext" $STR_EXT_VER $sysArch "An extension of std::string functionality in c++." > "$STREXT_BUILD_RELEASE_DIR/DEBIAN/control"
            cd "$STREXT_BUILD_RELEASE_DIR/.."
            dpkg-deb --build --root-owner-group $STREXT_RELEASE_DIR_NAME
            local releaseBuilt=$?
            if [[ $releaseBuilt -eq 0 ]] ; then
                mkdir -p "$PROJECT_ROOT/releases/linux/$sysArch/"
                cp "$STREXT_RELEASE_DIR_NAME.deb" "$PROJECT_ROOT/releases/linux/$sysArch/"
                releaseBuilt=$?
            fi
            cd $curDir
            return $releaseBuilt
        else
            >&2 echo "ERR: no release for $1"
            return 1     
        fi
    else
        >&2 echo "ERR: could not build and test $1"
        return 1
    fi
    return 1
}



