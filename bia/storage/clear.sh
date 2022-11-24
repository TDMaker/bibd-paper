for way in $(ls | grep 'k[0-9]'); do
    for csp in $(ls $way); do
        rm -rf $way/$csp/*
    done
done
