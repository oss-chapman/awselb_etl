
awselb_etl
=============

This is a parser for AWS ALB/ELB access log files written in C. 

* It parses the gz'd log files from amazon's Application Load Balancer (ALB) and 

* It loads 590k entries from 576 files in about 12 seconds.

* It can read .gz or unzipped files.

Installing
----------

I'm on debian. It should be portable on other systems. simply run `make` and this will build
`awselb_etl` You may need to install the packages `zlib1g-dev` and `libsqlite3-dev` or the
equivalent for your os. It uses gnu flex to build the lexer.
I've been running it directly from the build directory, you can install it in /usr/local/bin if you like.


Usage
-----
First, you need some AWS ELB/ALB log files. I'm not going to cover how to create those. 
see https://docs.aws.amazon.com/elasticloadbalancing/latest/application/load-balancer-access-logs.html

Next, get a copy of them from S3 down to your local system:

```sh
mkdir data; cd data

# copy all the logs from S3 down to here:
aws s3 sync s3://your_alb_bucket/path/AWSLogs/elasticloadbalancing/us-east-1/2018/04/10/ .
cd ..
```

Now run the etl. It will create the sqlite3 database if it doesn't already exist:

```sh
awselb_etl --batch 10000 data/*.gz
...
sqlite3_log.c: debug file data/815652239487_elasticloadbalancing_us-east-1_app.jira-load-balancer-act.bdcec31fd2fb89ec_20180410T1740Z_10.188.240.74_5hccy3f8.log.gz is rowid 426
batch commit 1180 records
... [repeats for all files in that directory]

```


Finally, we can run SQL against the database:

`sqlite3 elblog.sqlite3`
```sql
select count(*), user_agent from elblog group by user_agent order by count(*);
```

```sql
[...snip...]
7294|Apache-HttpClient/4.4.1 (Java/1.8.0_131)
9912|Apache-HttpClient/4.5.3 (Java/1.8.0_102)
16925|Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36
19347|Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36
21702|Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/11.1 Safari/605.1.15
23821|Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36
35283|-
178598|Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.119 Safari/537.36
199270|Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.119 Safari/537.36
```

There are lots of Chrome/Apple users at my office :)




Technical Details
-----------------
I wrote it to be modular so that another database backend could be plugged in.
Currently it writes them to a sqlite3 database. The DB agnostic interface is in `database_iface.h`

The database will be written to the databse file `elblog.sqlite3` If it doesn't exist, the database schema will be created.


The schema is fairly flat, there are three tables:

- `elblog_file` Contains the names of the ingested files and the time of ingest.
- `elblog_request` The breakdown of the request field for ease of searching.
- `elblog` The other columns from each elblog row, it has foreign keys to file and request

The actual sqlite3 schema is here:
```sql
CREATE TABLE elblog_file ( 
    id INTEGER PRIMARY KEY AUTOINCREMENT, 
    filename TEXT UNIQUE NOT NULL,
    ts_ingest FLOAT NOT NULL
);
```

```sql
CREATE TABLE elblog_request ( 
    id INTEGER PRIMARY KEY AUTOINCREMENT, 
    method TEXT NOT NULL, 
    url_protocol TEXT NOT NULL, 
    hostname TEXT NOT NULL, 
    port INTEGER NOT NULL, 
    path TEXT NOT NULL, 
    args TEXT NOT NULL, 
    protocol TEXT NOT NULL
);
```

```sql
CREATE TABLE elblog ( 
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    elblog_file_id REFERENCES elblog_file(id) ON DELETE CASCADE,
    elblog_file_lineno INTEGER NOT NULL,
    protocol TEXT NOT NULL,
    timestamp TEXT NOT NULL,
    elb TEXT NOT NULL,
    client_ipaddr TEXT NOT NULL,
    client_port INTEGER NOT NULL,
    target_ipaddr TEXT NOT NULL,
    target_port INTEGER NOT NULL,
    request_processing_time FLOAT NOT NULL,
    target_processing_time FLOAT NOT NULL,
    response_processing_time FLOAT NOT NULL,
    elb_status_code INTEGER NOT NULL,
    target_status_code INTEGER NOT NULL,
    received_bytes INTEGER NOT NULL,
    sent_bytes INTEGER NOT NULL,
    request_id REFERENCES request(id) ON DELETE RESTRICT,
    user_agent TEXT NOT NULL,
    ssl_cipher TEXT NOT NULL,
    ssl_protocol TEXT NOT NULL,
    target_group_arn TEXT NOT NULL,
    trace_id TEXT NOT NULL,
    domain_name TEXT NOT NULL,
    chosen_cert_arn TEXT NOT NULL,
    matched_rule_priority INTEGER NOT NULL
);
```
