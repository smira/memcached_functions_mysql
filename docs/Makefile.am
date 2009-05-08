INCLUDES = include

EXTRA_DIST = memc_get.pod\
	memc_set.pod\
	memc_append.pod\
	memc_prepend.pod\
	memc_increment.pod\
	memc_decrement.pod\
	memc_servers_set.pod\
	memc_list_behaviors.pod\
	memc_list_hash_types.pod\
	memc_list_distribution_types.pod\
	memc_servers_behavior_set.pod\
	memc_servers_behavior_get.pod\
	memc_behavior_set.pod\
	memc_behavior_get.pod\
	memc_udf_version.pod\
	memc_libmemcached_version.pod\
	memc_stats.pod

man_MANS = memc_get.3\
	memc_set.3\
	memc_append.3\
	memc_prepend.3\
	memc_increment.3\
	memc_decrement.3\
	memc_servers_set.3\
	memc_list_behaviors.3\
	memc_list_hash_types.3\
	memc_list_distribution_types.3\
	memc_servers_behavior_set.3\
	memc_servers_behavior_get.3\
	memc_behavior_set.3\
	memc_behavior_get.3\
	memc_udf_version.3\
	memc_libmemcached_version.3\
	memc_stat_get_keys.3\
	memc_stats.3

memc_get.3: memc_get.pod 
	pod2man  -r "" -s 3 memc_get.pod > memc_get.3
memc_get_by_key.3: memc_get.pod 
	pod2man  -r "" -s 3 memc_get.pod > memc_get_by_key.3
memc_add.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_add.3
memc_replace.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_replace.3
memc_cas.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_cas.3
memc_cas_by_key.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_cas_by_key.3
memc_set.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_set.3
memc_set_by_key.3: memc_set.pod 
	pod2man  -r "" -s 3 memc_set.pod > memc_set_by_key.3
memc_append.3: memc_append.pod 
	pod2man  -r "" -s 3 memc_append.pod > memc_append.3
memc_append_by_key.3: memc_append.pod 
	pod2man  -r "" -s 3 memc_append.pod > memc_append_by_key.3
memc_prepend.3: memc_prepend.pod 
	pod2man  -r "" -s 3 memc_prepend.pod > memc_prepend.3
memc_prepend_by_key.3: memc_prepend.pod 
	pod2man  -r "" -s 3 memc_prepend.pod > memc_prepend_by_key.3
memc_increment.3: memc_increment.pod 
	pod2man  -r "" -s 3 memc_increment.pod > memc_increment.3
memc_decrement.3: memc_decrement.pod 
	pod2man  -r "" -s 3 memc_decrement.pod > memc_decrement.3
memc_servers_set.3: memc_servers_set.pod 
	pod2man  -r "" -s 3 memc_servers_set.pod > memc_servers_set.3
memc_list_behaviors.3: memc_list_behaviors.pod 
	pod2man  -r "" -s 3 memc_list_behaviors.pod > memc_list_behaviors.3
memc_list_hash_types.3: memc_list_hash_types.pod 
	pod2man  -r "" -s 3 memc_list_hash_types.pod > memc_list_hash_types.3
memc_list_distribution_types.3: memc_list_distribution_types.pod 
	pod2man  -r "" -s 3 memc_list_distribution_types.pod > memc_list_distribution_types.3
memc_servers_behavior_set.3: memc_servers_behavior_set.pod 
	pod2man  -r "" -s 3 memc_servers_behavior_set.pod > memc_servers_behavior_set.3
memc_servers_behavior_get.3: memc_servers_behavior_get.pod 
	pod2man  -r "" -s 3 memc_servers_behavior_get.pod > memc_servers_behavior_get.3
memc_behavior_set.3: memc_behavior_set.pod 
	pod2man  -r "" -s 3 memc_behavior_set.pod > memc_behavior_set.3
memc_behavior_get.3: memc_behavior_get.pod 
	pod2man  -r "" -s 3 memc_behavior_get.pod > memc_behavior_get.3
memc_stats.3: memc_stats.pod 
	pod2man  -r "" -s 3 memc_stats.pod > memc_stats.3
memc_stat_get_keys.3: memc_stats.pod 
	pod2man  -r "" -s 3 memc_stats.pod > memc_stat_get_keys.3
memc_stat_get_value.3: memc_stats.pod 
	pod2man  -r "" -s 3 memc_stats.pod > memc_stat_get_value.3
memc_udf_version.3: memc_stats.pod 
	pod2man  -r "" -s 3 memc_stats.pod > memc_udf_version.3
memc_libmemcached_version.3: memc_stats.pod 
	pod2man  -r "" -s 3 memc_stats.pod > memc_libmemcached_version.3


clean:
	rm -f *.3
