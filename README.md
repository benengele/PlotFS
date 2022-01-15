# PlotFS
a fuse filesystem for efficiently storage of Chia plot files

PlotFS is not a traditional filesystem. It is mounted read only for compatiability 
with farming/harvesting. Adding/removing plotis is achieved via the `plotfs` command line tool.

PlotFS writes Chia plot files directly and contiguously to a raw block devices or partations.
Metadata, such as plot file locations and size are recorded in a separate "geomotroy" file stored on the OS drive. Plots can be split into "shards" and spread across mutipule disks to achieve maximum storage density.

# WARNING
This software is beta quality... at best. Use at your own risk. 
The author is not responsible for any lost data that may occur.

### Project Goals
Modern file systems, while amazing, are not really suited for storing Chia plots.
A filesystem designed for chia has much different usage pattern. It must:

* Have zero (or close to zero) space overhead
* Maintain operation upon the failure of any number of disks
* Loose the fewest number of plots possible upon loss of a disk
* Allow for unmacthed disk sizes
* Combine fractional remainder space across disk to maximize the number of plots
* Be fast enough

It doesn't care about:

* Directory structure
* Frequent deletes
* Large number of small files
* Rendundency
* Convenience
* Windows
* Humans in general

### Installing
Ubuntu:

Raspberry Pi:
TODO

### Getting started

Create a place to store our geomotry file
    $ sudo mkdir -p /var/local/plotfs
    $ sudo chown $USER /var/local/plotfs

Create the geomotry file
    $ plotfs --init

This will create a file in `/var/local/plotfs/plotfs.bin`
Note: It is VERY important that you back up this file! if you loose this file, you loose your plots!
(It may be possible to rebuild this file later by scanning the dirves/partitions But I have not yet written ta tool to do that)

Create a mount point
    $ sudo mkdir /farm

Mount the filesystem:
    $ sudo mount.plotfs /farm

Add this directory to chia client. Use the GUI or from yor chia install directory run: 
    $  . ./activate && chia plots add -d /farm

Add a disk or partition to the plotfs pool. Repeat for mutipule disks/partitions. Pools can be expanded at at time by adding more disks/partitions.
    $ chiafs --add-device /dev/disk/by-id/[your disk or partition]

Optional: Configure the filesystem to mount at boot. My personal favorite approach is using cron.
    $ sudo crontab -e
Add the line
    @reboot mount.plotfs /farm

Start adding plots:
    $ sudo plotfs --add-plot /path/to/plot.chia

## plotfs CLI usage

$ plotfs
--init
    Initialize the plotfs filesystem. Creates a file at `/var/local/plotfs/plotfs.bin`. Back this file up!
    combine with --force to clear the entire geomory file and start over.

--list-devices
    List the devices that are currently being used by plotfs

--add-device [device path] 
    Add a new device or partation to the filesystem.
    combine with --force to erase and reuse an existing device or partition

--remove-device [device id]
    Remove a device or partation from the filesystem.

--list-plots
    List the plots that are currently stored in the filesystem

--add-plot [plot path]
    Add a plot to the filesystem.

--remove-plot [plot id]
    Remove a plot from the filesystem.


$ mount.plotfs [mount point]
    Mounts the filesystem at the given mount point.

## FAQ

Q. Wow this is great! How can I give you all my Chia?
A. xch1hsyyclxn2v59ysd4n8nk577sduw64sg90nr8z26c3h8emq7magdqqzq9n5

Q. Will this work on rasperrry pi?
A. I havent tested yet, but it should. If it doesn't let me know, I'll fix it.

Q. Why do the filenmes not have the date?
A. plotfs does not have the concept of filenames. It uses the plot id and k value from the plot header as the file name. The plot header does not record the date created.

Q. What happens when/if chia releases plot compression?
A. You can compress a plot, the delete and add it back via the cli.. To minimize fragmentations its best to do this in a spacific order. I will document more if/when plot compression is available.

Q. I lost my config file! Can you release the recovery tool?
A. I haven't written it yet. PlotFS is just a side project and my day job keeps me busy.
    Greasing the wheels wouldn't hurt though: xch1hsyyclxn2v59ysd4n8nk577sduw64sg90nr8z26c3h8emq7magdqqzq9n5

Q. Windows when?
A. Literlly never. Use linux.