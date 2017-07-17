## Build information

Date: 14/07/2017 <br/>
Tag: v17.07_REL1.0 <br/>
Build System: Ubuntu 16.04.2 LTS <br/>
GCC Version: 5.3 <br/>

## Modifying kernel boot parameters
#### To modify kernel boot parameters, you can follow either of the below mentioned methods.
 - Edit the boot parameters in the grub menu entry for luvOS.<br/>
    - Select the luvOS entry on grub menu.
    - Press 'e'. 
    - Edit the parameters. <br/>
 - Modify the grub configuration file in the boot partition of luv-live-image-gpt.img. <br/>
     - sudo kpartx -a -v luv-live-image-gpt.img
     - sudo mount /dev/mapper/loop0p2 /mnt
     - sudo vim /mnt/EFI/BOOT/grub.cfg (Modify the luvOS boot entry)
     - sudo umount /mnt
     - sudo kpartx -d -v luv-live-image-gpt.img
