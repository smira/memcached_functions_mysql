drop table if exists urls;
create table urls (
  id int(3) not null,
  url varchar(64) not null default '',
  primary key (id)
  );

select memc_servers_set('localhost:11211');
select memc_set('urls:sequence', 0);

DELIMITER |

DROP TRIGGER IF EXISTS url_mem_insert |
CREATE TRIGGER url_mem_insert
BEFORE INSERT ON urls
FOR EACH ROW BEGIN
    SET NEW.id= memc_increment('urls:sequence'); 
    SET @mm= memc_set(concat('urls:',NEW.id), NEW.url);
END |

DROP TRIGGER IF EXISTS url_mem_update |
CREATE TRIGGER url_mem_update
BEFORE UPDATE ON urls
FOR EACH ROW BEGIN
    SET @mm= memc_replace(concat('urls:',OLD.id), NEW.url);
END |

DROP TRIGGER IF EXISTS url_mem_delete |
CREATE TRIGGER url_mem_delete
BEFORE DELETE ON urls
FOR EACH ROW BEGIN
    SET @mm= memc_delete(concat('urls:',OLD.id));
END |

DELIMITER ;

insert into urls (url) values ('http://google.com');
insert into urls (url) values ('http://lycos.com/');
insert into urls (url) values ('http://tripod.com/');
insert into urls (url) values ('http://microsoft.com/');
insert into urls (url) values ('http://slashdot.org');
insert into urls (url) values ('http://mysql.com');
select * from urls;

select memc_get('urls:1');
select memc_get('urls:2');
select memc_get('urls:3');
select memc_get('urls:4');
select memc_get('urls:5');
select memc_get('urls:6');

update urls set url= 'http://mysql.com/sun' where url = 'http://mysql.com';
select url from urls where url = 'http://mysql.com/sun';
select memc_get('urls:6');

delete from urls where url = 'http://microsoft.com/';
select * from urls where url='http://microsoft.com/';
select memc_get('urls:4');

