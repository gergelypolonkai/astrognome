#! /bin/sh

(echo -n "((nil . ((company-clang-arguments . ("
echo '"-I.."'
for line in `cat - | sed -e 's/\s\+/\n/g' | grep '^-I' | sort | uniq`
do
    echo '"'$line'"'
done
echo ")))))") > .dir-locals.el
