#! /bin/sh

output_line() {
    tld=$1
    char_count=$(echo -n "$tld" | wc -m)
    echo "  {${char_count}, \"${tld}\"},"
}

tlds_file=$(mktemp --suffix=.yml --tmpdir tlds-XXXX)
cctlds_file=$(mktemp --suffix=.in --tmpdir cctlds-XXXX)
gtlds_file=$(mktemp --suffix=.in --tmpdir gtlds-XXXX)
trap "rm -f ${tlds_file} ${cctlds_file} ${gtlds_file}" 0 2 3 15

curl -L -o "${tlds_file}" https://github.com/twitter/twitter-text/raw/master/conformance/tlds.yml
grep "is a valid country tld" "${tlds_file}" | awk '{print $3}' | while read tld; do output_line $tld; done | sort -k1.4n,2 > "${cctlds_file}"
grep "is a valid generic tld" "${tlds_file}" | awk '{print $3}' | while read tld; do output_line $tld; done | sort -k1.4n,2 > "${gtlds_file}"
cat src/data.h.in.1 "${gtlds_file}" src/data.h.in.2 "${cctlds_file}" src/data.h.in.3 > src/data.h