Muda
========================

Muda means useless in Japanese. This is a simple TODO list web application written in
C++ and Web Toolkit. 

Dependencies
========================

- [Boost](http://www.boost.org/)
- [Wt](http://www.webtoolkit.eu/wt)
- [Postgresql](http://www.postgresql.org/)

Building
========================

````bash
mkdir build
cd build
cmake ..
make 
````

Running
=========================

1. Create a database in Postgresql called 'muda' and create a user.
2. Copy the wt_config.xml.example and modify it to connect to the Postgresql database.
3. Copy the resources directory where you want the root directory to be.

Then execute

```bash
./muda --docroot=root --http-port=80 --http-address=0.0.0.0 --config wt_config.xml
```


License
=========================
This project is licensed under the GNU Affero GPL Version 3 license.
Read the COPY file about the specifics of the license.
