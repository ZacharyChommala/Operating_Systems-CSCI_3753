/*
  Name: Derek Gorthy
  SID: 102359021
  Date: 4/28/2016
  Class: CSCI 3753
  Assignment #5


  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.

*/

/*
  Help received from:
  Jack Dinkel
  Luke Meszar
  Reference Tutorial (12)

  I also referenced https://github.com/BenedictRM/CSCI-3753-PA4/blob/master/pa4-encfs.c
  for structure and error checking. No other code taken from this source
*/

// These macros given
#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR
#define PATH_MAX 1024

// Macros for temporary file and encryption flag
#define TEMP_FILE "/tmp/pa5"
#define XATTR_ENCRYPTED_FLAG "user.pa5-encfs.encrypted"

// Macros for do_crypt function
#define PASS_THROUGH -1
#define ENCRYPT 1
#define DECRYPT 0

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include "aes-crypt.h" // Include to omit error

// How I retrieve the private data of each file
#define ENCFS_DATA ((char**) fuse_get_context()->private_data)


// Pass in empty array -> append (root -> mirror) path
// Note that the mirror -> directory path is already saved
void new_path(char updated_path[PATH_MAX], const char *path){
	char * new_path = ENCFS_DATA[0];
	strcpy(updated_path, new_path);
	strcat(updated_path, path);
}


static int encfs_getattr(const char *path, struct stat *stbuf)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = lstat(updated_path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_access(const char *path, int mask)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = access(updated_path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_readlink(const char *path, char *buf, size_t size)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = readlink(updated_path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int encfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(updated_path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int encfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(updated_path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(updated_path, mode);
	else
		res = mknod(updated_path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_mkdir(const char *path, mode_t mode)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = mkdir(updated_path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_unlink(const char *path)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = unlink(updated_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_rmdir(const char *path)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = rmdir(updated_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_chmod(const char *path, mode_t mode)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = chmod(updated_path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_chown(const char *path, uid_t uid, gid_t gid)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = lchown(updated_path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_truncate(const char *path, off_t size)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = truncate(updated_path, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_utimens(const char *path, const struct timespec ts[2])
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(updated_path, tv);
	if (res == -1)
		return -errno;

	return 0;
}


/*
  encfs_open is called when FUSE needs to open a file. Explanation of the function
  is commented above each line.
*/
static int encfs_open(const char *path, struct fuse_file_info *fi)
{
	// Local variables
	(void) fi;
	char updated_path[PATH_MAX]; // The full path to directory will be stored here
	new_path(updated_path,path); // Update full path from path

	int action = PASS_THROUGH; // Initially do nothing to file
	ssize_t xattr_len; // Used to store the return of getxattr()
	char value[5]; // The max size of "false" or "true" is 5, initialize array to store flag

	int res;

	res = open(updated_path, fi->flags);
	if (res == -1)
		return -errno;
	close(res);

	// Find the length of the attribute. If unitialized (not encrypted), will be -1
	xattr_len = getxattr(updated_path, XATTR_ENCRYPTED_FLAG, value, 5);
	// Array indeces 0, 1, 2 must be checked in case random date store (by chance)
	// evaluates to 't', 'r', 'u'. Likelihood that all three values random evaluate
	// to the expected letters is extremely low.
	if(xattr_len != -1 && value[0] == 't' && value[1] == 'r' && value[2] == 'u'){

		// Open the temp file and mirrored directory path
		// Data flows from updated_path -> temp
		FILE * temp = fopen(TEMP_FILE, "w");
		FILE * inf = fopen(updated_path, "r");

		// Pass through on first round
		if(!do_crypt(inf, temp, action, ENCFS_DATA[1])){
			fprintf(stderr, "first do_crypt failed.\n");
			return -errno;
		}

		// Close both files
		fclose(temp);
		fclose(inf);

		// Reopen both files, now with switched arguments
		// Data flows from temp -> updated_path this time
		// Note that "temp" acts as a translator when we want to decrypt
		temp = fopen(TEMP_FILE, "r");
		inf = fopen(updated_path, "w");	
		// Now change our action to decrypt for decryption
		action = DECRYPT;

		// Now it is time to decrypt the temp file and write...
		if(!do_crypt(temp, inf, action, ENCFS_DATA[1])){
			fprintf(stderr, "second do_crypt failed.\n");
			return -errno;
		}

		// Close both files again
		fclose(temp);
		fclose(inf);
		// Clean up our temp file to prevent memory leaking
		remove(TEMP_FILE);

		// This file has now been decrypted, set flag to false (not decrypted)
		if(setxattr(updated_path, XATTR_ENCRYPTED_FLAG, "false", 5, 0)){
			fprintf(stderr, "decrypting failed.\n");
			return -errno;
		}
	}

	return 0;
}

static int encfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int fd;
	int res;

	(void) fi;
	fd = open(updated_path, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int encfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int fd;
	int res;

	(void) fi;
	fd = open(updated_path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int encfs_statfs(const char *path, struct statvfs *stbuf)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res;

	res = statvfs(updated_path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

/*
  
*/
static int encfs_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
	
	// As in every function, update the path...
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
    (void) fi;

    int res;
    res = creat(updated_path, mode);
    if(res == -1)
	return -errno;

    close(res);

    // Set the flag to false.. it is not encrypted
	if(setxattr(updated_path, XATTR_ENCRYPTED_FLAG, "false", 5, 0)){
		fprintf(stderr, "encrypting failed.\n");
		return -errno;
	}

    return 0;
}

/*
  encfs_release is called when FUSE is finished with a file. Explanation of the function
  is commented above each line.
*/
static int encfs_release(const char *path, struct fuse_file_info *fi)
{
	// Local variables
	(void) fi;
	char updated_path[PATH_MAX]; // This is where the full file path will be stored
	new_path(updated_path,path); // Get updated path from path

	int action = ENCRYPT; // We initially want to encrypt this file
	char value[5]; // This is where the encryption flag will be stored
	ssize_t xattr_len; 
	
	// Find the length of the attribute. If unitialized (not encrypted), will be -1
	xattr_len = getxattr(updated_path, XATTR_ENCRYPTED_FLAG, value, 5);
	// Array indeces 0, 1, 2 must be checked in case random date store (by chance)
	// evaluates to 'f', 'a', 'l'. Likelihood that all three values random evaluate
	// to the expected letters is extremely low.
	if(xattr_len != -1 && value[0] == 'f' && value[1] == 'a' && value[2] == 'l'){
		// Open both files with permissions
		// Data flows from updated_path -> temp
		FILE * temp = fopen(TEMP_FILE, "w");
		FILE * inf = fopen(updated_path, "r");

		// Initially encrypt the data
		if(!do_crypt(inf, temp, action, ENCFS_DATA[1])){
			fprintf(stderr, "first do_crypt failed.\n");
			return -errno;
		}

		// Close both files
		fclose(temp);
		fclose(inf);

		// Open both files again
		// Data flows from temp -> updated_path this time
		// Note that "temp" acts as a translator when we want to encrypt
		temp = fopen(TEMP_FILE, "r");
		inf = fopen(updated_path, "w");
		action = PASS_THROUGH;

		// Encrypt the file
		if(!do_crypt(temp, inf, action, ENCFS_DATA[1])){
			fprintf(stderr, "second do_crypt failed.\n");
			return -errno;
		}

		// Close both files
		fclose(temp);
		fclose(inf);
		// Remove the temp file to prevent memory links
		remove(TEMP_FILE);

		// Now that the file has been re-encrypted..
		// We now need to set the flag back to true
		if(setxattr(updated_path, XATTR_ENCRYPTED_FLAG, "true", 5, 0)){
			fprintf(stderr, "encrypting failed.\n");
			return -errno;
		}

	}

	return 0;
}

static int encfs_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	(void) updated_path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int encfs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res = lsetxattr(updated_path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int encfs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);
	int res = lgetxattr(updated_path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int encfs_listxattr(const char *path, char *list, size_t size)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);

	int res = llistxattr(updated_path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int encfs_removexattr(const char *path, const char *name)
{
	char updated_path[PATH_MAX];
	new_path(updated_path,path);

	int res = lremovexattr(updated_path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

// Basic usage function in case user doesn't execute correctly
// Taken from http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/
void bb_usage()
{
    fprintf(stderr, "usage:  bbfs [FUSE and mount options] rootDir mountPoint\n");
    exit(0);
}

static struct fuse_operations encfs_oper = {
	.getattr	= encfs_getattr,
	.access		= encfs_access,
	.readlink	= encfs_readlink,
	.readdir	= encfs_readdir,
	.mknod		= encfs_mknod,
	.mkdir		= encfs_mkdir,
	.symlink	= encfs_symlink,
	.unlink		= encfs_unlink,
	.rmdir		= encfs_rmdir,
	.rename		= encfs_rename,
	.link		= encfs_link,
	.chmod		= encfs_chmod,
	.chown		= encfs_chown,
	.truncate	= encfs_truncate,
	.utimens	= encfs_utimens,
	.open		= encfs_open,
	.read		= encfs_read,
	.write		= encfs_write,
	.statfs		= encfs_statfs,
	.create         = encfs_create,
	.release	= encfs_release,
	.fsync		= encfs_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= encfs_setxattr,
	.getxattr	= encfs_getxattr,
	.listxattr	= encfs_listxattr,
	.removexattr	= encfs_removexattr,
#endif
};

int main(int argc, char *argv[])
{
	umask(0);
	// Check for correct usage and make sure no path will break the program
	// Taken from http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/
    if ((argc < 4) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-') || (argv[argc-3][0] == '-')){
		bb_usage();
	}

	// Create an array to store the private data, this was used in place of a struct for ease of use
	char *private_data[2];
	// Save the real path from root to base
	private_data[0] = realpath(argv[argc-2], NULL);
	// Save password for encrypting...
	private_data[1] = argv[argc-3];

	// Now need to reorder the arguments
	// Initially taken from http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/
	// Got help for last argument from Jack Dinkel
	argv[argc-3] = argv[argc-1];
	argv[argc-1] = NULL;
	argv[argc-2] = NULL;
	argc -= 2;

	// Instead of NULL, now need to pass in the private_data we just wrote
	return fuse_main(argc, argv, &encfs_oper, private_data);
}
