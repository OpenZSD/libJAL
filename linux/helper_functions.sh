ABSOLUTE_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function inList() {
    local list=$1
    local item=$2

    local notFound=1
    if [[ $list =~ (^|[[:space:]])"$item"($|[[:space:]]) ]] ; then
        notFound=0
    else
        notFound=1
  fi
  return $notFound
}

function printDebianCtl(){
    echo -e "\
Package: $1 \n\
Version: $2 \n\
Architecture: $3 \n\
Maintainer: OpenZSD <open_help@zsoftdevelopment.com> \n\
Description: $4 \n"          
}
