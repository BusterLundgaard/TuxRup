DIR="$1"
NAME="$2"
BUILD_ID=$(readelf -n "${DIR}/${NAME}" | awk '/Build ID/ {print $3}')
URL="https://debuginfod.archlinux.org/buildid/${BUILD_ID}/debuginfo"
curl -s "$URL" > "./symbols/${NAME}_symbols" 
echo "done finding debug symbols"
