/* SPDX-License-Identifier: LGPL-2.1+ */

#include "path-lookup.h"
#include "strv.h"
#include "tests.h"
#include "unit-file.h"

static void test_unit_validate_alias_symlink_and_warn(void) {
        log_info("/* %s */", __func__);

        assert_se(unit_validate_alias_symlink_and_warn("/path/a.service", "/other/b.service") == 0);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a.service", "/other/b.socket") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a.service", "/other/b.foobar") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@.service", "/other/b@.service") == 0);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@.service", "/other/b@.socket") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@XXX.service", "/other/b@YYY.service") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@XXX.service", "/other/b@YYY.socket") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@.service", "/other/b@YYY.service") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@XXX.service", "/other/b@XXX.service") == 0);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@XXX.service", "/other/b@.service") == 0);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@.service", "/other/b.service") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a.service", "/other/b@.service") == -EXDEV);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a@.slice", "/other/b.slice") == -EINVAL);
        assert_se(unit_validate_alias_symlink_and_warn("/path/a.slice", "/other/b.slice") == -EINVAL);
}

static void test_unit_file_build_name_map(void) {
        _cleanup_(lookup_paths_free) LookupPaths lp = {};
        _cleanup_hashmap_free_ Hashmap *unit_ids = NULL;
        _cleanup_hashmap_free_ Hashmap *unit_names = NULL;
        Iterator i;
        const char *k, *dst;
        char **v;

        assert_se(lookup_paths_init(&lp, UNIT_FILE_SYSTEM, 0, NULL) >= 0);

        assert_se(unit_file_build_name_map(&lp, &unit_ids, &unit_names, NULL) == 0);

        HASHMAP_FOREACH_KEY(dst, k, unit_ids, i)
                log_info("ids: %s → %s", k, dst);

        HASHMAP_FOREACH_KEY(v, k, unit_names, i) {
                _cleanup_free_ char *j = strv_join(v, ", ");
                log_info("aliases: %s ← %s", k, j);
        }
}

int main(int argc, char **argv) {
        test_setup_logging(LOG_DEBUG);

        test_unit_validate_alias_symlink_and_warn();
        test_unit_file_build_name_map();

        return 0;
}
