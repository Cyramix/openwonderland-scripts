#!/bin/bash 

# save_wfs.sh: a script that copies the content of a WFS snapshot and all associated artwork
# into a tar file.
#
# usage: save_wfs.sh input_dir (output_dir)
#   input_dir: the wfs snapshot to save a copy of
#   output_dir: the optional directory to write output to. If no directory is specified, the
#               current directory will be used

grep=grep
tar=tar

# copy all content referred to with wlcontent:// URLs
copy_content () {
    find $origdir -exec $grep "wlcontent://" {} \; | while read FILE; do
        i=`echo $FILE | sed -e 's|.*wlcontent://users@.*\:[0-9]*/\(.*\)<.*|\1|' | sed -e 's|.*wlcontent://users/\(.*\)<.*|\1|'`

        # find directory name
        dir=`echo $i | sed -e 's|\(.*\)/.*|\1|'` 
        mkdir -p $tmpdir/run/content/users/$dir
 
        echo "Copying content from $wldir/run/content/users/$i to $tmpdir/run/content/users/$dir"
        cp "$wldir/run/content/users/$i" "$tmpdir/run/content/users/$i"
    done
}

# copy all files associated with a deployed model
copy_models () {
    find $origdir -exec $grep deployedModelURL {} \; | while read FILE; do
        i=`echo $FILE | cut -d ':' -f 3 | sed -e 's|[0-9]*/\(.*\)/.*</.*>|/\1|'`

        echo "Copying model from $wldir/run/content/users/$i to $tmpdir/run/content/users"

        mkdir -p "$tmpdir/run/content/users/$i"
        cp -R "$wldir/run/content/users/$i"/* "$tmpdir/run/content/users/$i"
    done
}

# first argument: directory to take a snapshot of
origdir=$1

# second argument: directory to write to (optional)
if [ $2 ]; then
    outdir=$2
else
    outdir=`pwd`
fi

# find the Wonderland directory this wfs is a child of
wldir=`echo $1 | sed -e 's|\(.*\)/wfs/.*|\1|'`
echo "Wonderland directory $wldir"

# create the temporary directory to copy to
tmpdir=/tmp/wlsave
rm -rf $tmpdir

# create subdirectories we will need
mkdir -p $tmpdir/wfs/snapshots
mkdir -p $tmpdir/run/content/users

# copy WFS files -- this assumes a WFS snapshot
echo "Copying WFS from $1 to $tmpdir/wfs/snapshots"
cp -R $1 $tmpdir/wfs/snapshots

# copy all content from the content repository
copy_content

# special-case copying of files related to deployed models
copy_models

# determine the output directory and file name
outfile=`echo $1 | sed -e 's|.*/\(.*\)|\1|'`-`date +%m%d%Y`.tgz

# create the output file by tarring the temporary directory
echo "Writing to $outfile"
cd $tmpdir
$tar czf $outdir/$outfile *
