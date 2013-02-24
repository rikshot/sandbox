#pragma once

#include <boost\shared_ptr.hpp>

#include "vector.hpp"
#include "object.hpp"

namespace sandbox {

class contact {
public:
	contact(boost::shared_ptr<object> const & a, boost::shared_ptr<object> const & b, vector const & ap, vector const & bp, vector const & normal) : a_(a), b_(b), ap_(ap), bp_(bp), normal_(normal) {
	}

	boost::shared_ptr<object> a() const {
		return a_;
	}

	boost::shared_ptr<object> b() const {
		return b_;
	}

	vector const & ap() const {
		return ap_;
	}

	vector const & bp() const {
		return bp_;
	}

	vector const & normal() const {
		return normal_;
	}

	double relative_velocity() const {
		vector const ra(a_->position() - ap_);
		vector const rb(b_->position() - bp_);
		vector const vab(b_->linear_velocity() - rb.cross(b_->angular_velocity()) - a_->linear_velocity() + ra.cross(a_->angular_velocity()));
		return vab.dot(normal_);
	}

private:
	boost::shared_ptr<object> a_;
	boost::shared_ptr<object> b_;
	vector ap_;
	vector bp_;
	vector normal_;
};

}